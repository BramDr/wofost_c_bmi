#include "bmi_wofost.h"
#include "math_utils.h"
#include "simulation.h"
#include "time_utils.h"
#include <bmi.h>
#include <string.h>

static const BmiVar BMI_INPUT_VARS[BMI_NINPUTS] = {
    /* name              grid     type      units          itemsize location */
    // Static
    {BMI_INPUT_SOIL_AERATED, "in_soil_aerated", GRID_ID, FLOAT_TYPE,
     "m m_soil-1", sizeof(float), GRID_LOCATION},
    {BMI_INPUT_SOIL_FIELD_CAPACITY, "in_soil_field_capacity", GRID_ID,
     FLOAT_TYPE, "m m_soil-1", sizeof(float), GRID_LOCATION},
    {BMI_INPUT_SOIL_WILTING_POINT, "in_soil_wilting_point", GRID_ID, FLOAT_TYPE,
     "m m_soil-1", sizeof(float), GRID_LOCATION},
    {BMI_INPUT_SOIL_SATURATED, "in_soil_saturated", GRID_ID, FLOAT_TYPE,
     "m m_soil-1", sizeof(float), GRID_LOCATION},
    // Dynamic
    {BMI_INPUT_PRECIPITATION, "in_precipitation", GRID_ID, FLOAT_TYPE,
     "m day-1", sizeof(float), GRID_LOCATION},
    {BMI_INPUT_MAXIMUM_TEMPERATURE, "in_maximum_temperature", GRID_ID,
     FLOAT_TYPE, "degrees Celsius", sizeof(float), GRID_LOCATION},
    {BMI_INPUT_MINIMUM_TEMPERATURE, "in_minimum_temperature", GRID_ID,
     FLOAT_TYPE, "degrees Celsius", sizeof(float), GRID_LOCATION},
    {BMI_INPUT_WIND_SPEED, "in_wind_speed", GRID_ID, FLOAT_TYPE, "m s-1",
     sizeof(float), GRID_LOCATION},
    {BMI_INPUT_SHORTWAVE_RADIATION, "in_shortwave_radiation", GRID_ID,
     FLOAT_TYPE, "W m-2", sizeof(float), GRID_LOCATION},
    {BMI_INPUT_VAPOR_PRESSURE, "in_vapor_pressure", GRID_ID, FLOAT_TYPE, "Pa",
     sizeof(float), GRID_LOCATION},
    // Dynamic per crop
    {BMI_INPUT_SOIL_SATURATION, "in_soil_saturation", GRID_ID, FLOAT_TYPE,
     "m m_soil-1", sizeof(float), GRID_LOCATION},
};

static const BmiVar BMI_OUTPUT_VARS[BMI_NOUTPUTS] = {
    /* name         grid     type      units     itemsize        location */
    {BMI_OUTPUT_LEAF_AREA_INDEX, "out_leaf_area_index", GRID_ID, FLOAT_TYPE,
     "m2 m-2", sizeof(float), GRID_LOCATION},
    {BMI_OUTPUT_ROOT_DEPTH, "out_root_depth", GRID_ID, FLOAT_TYPE, "cm",
     sizeof(float), GRID_LOCATION},
    {BMI_OUTPUT_EVAPOTRANSPIRATION, "out_evapotranspiration", GRID_ID,
     FLOAT_TYPE, "cm", sizeof(float), GRID_LOCATION},
};

/* -----------------------------------------------------------------------
 * Supporting function implementations
 * ----------------------------------------------------------------------- */

static size_t FindCropIndex(const char *name) {
  for (size_t i = 0; i < CropSize; i++) {
    size_t len = strlen(CropNames[i]);
    if (strncmp(name, CropNames[i], len) == 0 && name[len] == '_') {
      return i;
    }
  }
  return CropSize; // Not found
}

static const BmiVar *FindBmiInputVariable(const char *name) {
  for (int i = 0; i < BMI_NINPUTS; i++) {
    if (strcmp(BMI_INPUT_VARS[i].name, name) == 0) {
      return &BMI_INPUT_VARS[i];
    }
  }
  return NULL;
}

static const BmiVar *FindBmiOutputVariable(const char *name) {
  for (int i = 0; i < BMI_NOUTPUTS; i++) {
    if (strcmp(BMI_OUTPUT_VARS[i].name, name) == 0) {
      return &BMI_OUTPUT_VARS[i];
    }
  }
  return NULL;
}

static const BmiVar *FindBmiVariable(const char *name) {
  const BmiVar *input_var = FindBmiInputVariable(name);
  if (input_var != NULL) {
    return input_var;
  }
  const BmiVar *output_var = FindBmiOutputVariable(name);
  if (output_var != NULL) {
    return output_var;
  }
  return NULL;
}

/* -----------------------------------------------------------------------
 * BMI active function implementations
 * ----------------------------------------------------------------------- */

static int Initialize(struct Bmi *self, const char *config_file) {
  UNUSED(self);
  InitializeSimulation(config_file);
  return BMI_SUCCESS;
}
static int Update(struct Bmi *self) {
  UNUSED(self);
  UpdateSimulation();
  return BMI_SUCCESS;
}
static int Update_until(struct Bmi *self, double then) {
  UNUSED(self);
  UNUSED(then);
  return BMI_FAILURE;
}
static int Finalize(struct Bmi *self) {
  UNUSED(self);
  FinalizeSimulation();
  return BMI_SUCCESS;
}

static int Get_component_name(struct Bmi *self, char *name) {
  UNUSED(self);
  strncpy(name, "wofost", BMI_MAX_COMPONENT_NAME);
  return BMI_SUCCESS;
}
static int Get_input_item_count(struct Bmi *self, int *count) {
  UNUSED(self);
  *count = BMI_NINPUTS;
  return BMI_SUCCESS;
}
static int Get_output_item_count(struct Bmi *self, int *count) {
  UNUSED(self);
  *count = BMI_NOUTPUTS;
  return BMI_SUCCESS;
}
static int Get_input_var_names(struct Bmi *self, char **names) {
  UNUSED(self);
  for (int i = 0; i < BMI_NINPUTS; i++) {
    strncpy(names[i], BMI_INPUT_VARS[i].name, BMI_MAX_VAR_NAME);
  }
  return BMI_SUCCESS;
}
static int Get_output_var_names(struct Bmi *self, char **names) {
  UNUSED(self);
  for (int i = 0; i < BMI_NOUTPUTS; i++) {
    strncpy(names[i], BMI_OUTPUT_VARS[i].name, BMI_MAX_VAR_NAME);
  }
  return BMI_SUCCESS;
}

static int Get_grid_rank(struct Bmi *self, int grid, int *rank) {
  UNUSED(self);
  if (grid != GRID_ID) {
    return BMI_FAILURE;
  }
  if (NR_DOMAIN_DIMENSIONS <= 0) {
    return BMI_FAILURE;
  }
  *rank = NR_DOMAIN_DIMENSIONS;
  return BMI_SUCCESS;
}
static int Get_grid_size(struct Bmi *self, int grid, int *size) {
  UNUSED(self);
  if (grid != GRID_ID) {
    return BMI_FAILURE;
  }
  *size = 1;
  for (int i = 0; i < NR_DOMAIN_DIMENSIONS; i++) {
    if (DomainShape[i] <= 0) {
      return BMI_FAILURE;
    }
    *size *= DomainShape[i];
  }
  return BMI_SUCCESS;
}
static int Get_grid_type(struct Bmi *self, int grid, char *type) {
  UNUSED(self);
  if (grid != GRID_ID) {
    return BMI_FAILURE;
  }
  strncpy(type, GRID_TYPE, BMI_MAX_TYPE_NAME);
  return BMI_SUCCESS;
}
static int Get_grid_shape(struct Bmi *self, int grid, int *shape) {
  UNUSED(self);
  if (grid != GRID_ID) {
    return BMI_FAILURE;
  }
  for (int i = 0; i < NR_DOMAIN_DIMENSIONS; i++) {
    if (DomainShape[i] <= 0) {
      return BMI_FAILURE;
    }
    shape[i] = DomainShape[i];
  }
  return BMI_SUCCESS;
}
static int Get_grid_spacing(struct Bmi *self, int grid, double *spacing) {
  UNUSED(self);
  if (grid != GRID_ID) {
    return BMI_FAILURE;
  }
  if (Resolution <= 0) {
    return BMI_FAILURE;
  }
  for (int i = 0; i < NR_DOMAIN_DIMENSIONS; i++) {
    spacing[i] = Resolution;
  }
  return BMI_SUCCESS;
}
static int Get_grid_origin(struct Bmi *self, int grid, double *origin) {
  UNUSED(self);
  if (grid != GRID_ID) {
    return BMI_FAILURE;
  }
  if (NR_DOMAIN_DIMENSIONS != 2 || Latitudes == NULL || Longitudes == NULL) {
    return BMI_FAILURE;
  }
  origin[0] = Latitudes[0];
  origin[1] = Longitudes[0];
  return BMI_SUCCESS;
}

static int Get_var_grid(struct Bmi *self, const char *name, int *grid) {
  UNUSED(self);
  const BmiVar *v = FindBmiVariable(name);
  if (v == NULL)
    return BMI_FAILURE;
  *grid = v->grid;
  return BMI_SUCCESS;
}
static int Get_var_type(struct Bmi *self, const char *name, char *type) {
  UNUSED(self);
  const BmiVar *v = FindBmiVariable(name);
  if (v == NULL)
    return BMI_FAILURE;
  strncpy(type, v->type, BMI_MAX_TYPE_NAME);
  return BMI_SUCCESS;
}
static int Get_var_units(struct Bmi *self, const char *name, char *units) {
  UNUSED(self);
  const BmiVar *v = FindBmiVariable(name);
  if (v == NULL)
    return BMI_FAILURE;
  strncpy(units, v->units, BMI_MAX_UNITS_NAME);
  return BMI_SUCCESS;
}
static int Get_var_itemsize(struct Bmi *self, const char *name, int *size) {
  UNUSED(self);
  const BmiVar *v = FindBmiVariable(name);
  if (v == NULL)
    return BMI_FAILURE;
  *size = v->itemsize;
  return BMI_SUCCESS;
}
static int Get_var_nbytes(struct Bmi *self, const char *name, int *nbytes) {
  UNUSED(self);
  const BmiVar *v = FindBmiVariable(name);
  int grid_size;
  if (v == NULL)
    return BMI_FAILURE;
  if (Get_grid_size(self, v->grid, &grid_size) != BMI_SUCCESS)
    return BMI_FAILURE;
  *nbytes = v->itemsize * grid_size;
  return BMI_SUCCESS;
}
static int Get_var_location(struct Bmi *self, const char *name,
                            char *location) {
  UNUSED(self);
  const BmiVar *v = FindBmiVariable(name);
  if (v == NULL)
    return BMI_FAILURE;
  strncpy(location, v->location, BMI_MAX_VAR_NAME);
  return BMI_SUCCESS;
}

static int Get_current_time(struct Bmi *self, double *time) {
  UNUSED(self);
  *time = difftime(CurrentTime, REFERENCE_TIME_T);
  return BMI_SUCCESS;
}
static int Get_start_time(struct Bmi *self, double *time) {
  UNUSED(self);
  time_t time_time_t = timegm_portable(&Start);
  if (time_time_t == -1) {
    return BMI_FAILURE;
  }
  *time = difftime(time_time_t, REFERENCE_TIME_T);
  return BMI_SUCCESS;
}
static int Get_end_time(struct Bmi *self, double *time) {
  UNUSED(self);
  time_t time_time_t = timegm_portable(&End);
  if (time_time_t == -1) {
    return BMI_FAILURE;
  }
  *time = difftime(time_time_t, REFERENCE_TIME_T);
  return BMI_SUCCESS;
}
static int Get_time_units(struct Bmi *self, char *units) {
  UNUSED(self);
  strncpy(units, "seconds since 1970-01-01 00:00:00", BMI_MAX_UNITS_NAME);
  return BMI_SUCCESS;
}
static int Get_time_step(struct Bmi *self, double *time_step) {
  UNUSED(self);
  if (TIME_STEP <= 0) {
    return BMI_FAILURE;
  }
  *time_step = TIME_STEP;
  return BMI_SUCCESS;
}

static int Get_value(struct Bmi *self, const char *name, void *dest) {
  UNUSED(self);

  // Check if the name prefix matches any of the crop names
  // E.g., maize_leaf_area_index, wheat_root_depth, etc.
  size_t crop_index = FindCropIndex(name);
  if (crop_index == CropSize) {
    return BMI_FAILURE;
  }

  char var_name[BMI_MAX_VAR_NAME];
  size_t crop_name_len = strlen(CropNames[crop_index]);
  strncpy(var_name, name + crop_name_len + 1, BMI_MAX_VAR_NAME - 1);
  var_name[BMI_MAX_VAR_NAME - 1] = '\0';
  const BmiVar *v = FindBmiOutputVariable(var_name);
  if (v == NULL) {
    return BMI_FAILURE;
  }

  float *dest_float = (float *)dest;
  for (size_t i = 0; i < ActiveSize[crop_index]; i++) {
    const size_t index = ActiveIndex[crop_index][i];
    const SimUnit *unit = &SimGrid[crop_index][i];

    switch (v->index) {
    case BMI_OUTPUT_LEAF_AREA_INDEX:
      dest_float[index] = unit->crp.st.LAI;
      break;
    case BMI_OUTPUT_ROOT_DEPTH:
      dest_float[index] = unit->crp.st.RootDepth;
      break;
    case BMI_OUTPUT_EVAPOTRANSPIRATION:
      dest_float[index] = unit->soil.rt.EvapSoil + unit->soil.rt.Transpiration +
                          unit->soil.rt.EvapWater;
      break;
    default:
      return BMI_FAILURE;
    }
  }
  return BMI_SUCCESS;
}

static int Set_value(struct Bmi *self, const char *name, void *src) {
  UNUSED(self);

  const BmiVar *v;

  size_t crop_index = CropSize;
  v = FindBmiOutputVariable(name);

  if (v == NULL) {
    // Check if the name prefix matches any of the crop names
    crop_index = FindCropIndex(name);
    if (crop_index == CropSize) {
      return BMI_FAILURE;
    }

    char var_name[BMI_MAX_VAR_NAME];
    size_t crop_name_len = strlen(CropNames[crop_index]);
    strncpy(var_name, name + crop_name_len + 1, BMI_MAX_VAR_NAME - 1);
    var_name[BMI_MAX_VAR_NAME - 1] = '\0';
    v = FindBmiOutputVariable(var_name);
    if (v == NULL) {
      return BMI_FAILURE;
    }

    // Values provided per crop:
    // - BMI_INPUT_SOIL_SATURATION
    switch (v->index) {
    case BMI_INPUT_SOIL_SATURATION:
      break;
    default:
      return BMI_FAILURE;
    }
  }

  float *src_float = (float *)src;
  switch (v->index) {
  // Static
  case BMI_INPUT_SOIL_AERATED:
    for (size_t i = 0; i < CropSize; i++) {
      for (size_t j = 0; j < ActiveSize[i]; j++) {
        const size_t index = ActiveIndex[i][j];
        SimUnit *unit = &SimGrid[i][j];
        unit->soil.ct.CriticalSoilAirC = src_float[index]; // m m_soil-1
      }
    }
    break;
  case BMI_INPUT_SOIL_FIELD_CAPACITY:
    for (size_t i = 0; i < CropSize; i++) {
      for (size_t j = 0; j < ActiveSize[i]; j++) {
        const size_t index = ActiveIndex[i][j];
        SimUnit *unit = &SimGrid[i][j];
        unit->soil.ct.MoistureFC = src_float[index]; // m m_soil-1
      }
    }
    break;
  case BMI_INPUT_SOIL_WILTING_POINT:
    for (size_t i = 0; i < CropSize; i++) {
      for (size_t j = 0; j < ActiveSize[i]; j++) {
        const size_t index = ActiveIndex[i][j];
        SimUnit *unit = &SimGrid[i][j];
        unit->soil.ct.MoistureWP = src_float[index]; // m m_soil-1
      }
    }
    break;
  case BMI_INPUT_SOIL_SATURATED:
    for (size_t i = 0; i < CropSize; i++) {
      for (size_t j = 0; j < ActiveSize[i]; j++) {
        const size_t index = ActiveIndex[i][j];
        SimUnit *unit = &SimGrid[i][j];
        unit->soil.ct.MoistureSAT = src_float[index]; // m m_soil-1
      }
    }
    break;
    // Dynamic
  case BMI_INPUT_PRECIPITATION:
    for (size_t i = 0; i < DomainSize; i++) {
      DomUnit *grid = &DomGrid[i];
      grid->met.Rain = roundz2(100 * src_float[i]); // m day-1 to cm day-1
    }
    break;
  case BMI_INPUT_MAXIMUM_TEMPERATURE:
    for (size_t i = 0; i < DomainSize; i++) {
      DomUnit *grid = &DomGrid[i];
      grid->met.Tmax = roundz1(src_float[i]); // degrees Celsius
    }
    break;
  case BMI_INPUT_MINIMUM_TEMPERATURE:
    for (size_t i = 0; i < DomainSize; i++) {
      DomUnit *grid = &DomGrid[i];
      grid->met.Tmin = roundz1(src_float[i]); // degrees Celsius
    }
    break;
  case BMI_INPUT_SHORTWAVE_RADIATION:
    for (size_t i = 0; i < DomainSize; i++) {
      DomUnit *grid = &DomGrid[i];
      grid->met.Radiation =
          roundz1(86400 * src_float[i]); // W m-2 to J m-2 day-1
    }
    break;
  case BMI_INPUT_WIND_SPEED:
    for (size_t i = 0; i < DomainSize; i++) {
      DomUnit *grid = &DomGrid[i];
      grid->met.Windspeed = roundz1(src_float[i]); // m s-1
    }
    break;
  case BMI_INPUT_VAPOR_PRESSURE:
    for (size_t i = 0; i < DomainSize; i++) {
      DomUnit *grid = &DomGrid[i];
      grid->met.Vapour = roundz1(0.01 * src_float[i]); // Pa to hPa
    }
    break;
    // Dynamic per crop
  case BMI_INPUT_SOIL_SATURATION:
    for (size_t i = 0; i < ActiveSize[crop_index]; i++) {
      const size_t index = ActiveIndex[crop_index][i];
      SimUnit *unit = &SimGrid[crop_index][i];
      unit->soil.st.Moisture = src_float[index]; // m m_soil-1
    }
    break;
  default:
    return BMI_FAILURE;
  }
  return BMI_SUCCESS;
}

/* -----------------------------------------------------------------------
 * BMI inactive function implementations
 * -----------------------------------------------------------------------
 */

static int Get_grid_x(struct Bmi *self, int grid, double *x) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(x);
  return BMI_FAILURE;
}
static int Get_grid_y(struct Bmi *self, int grid, double *y) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(y);
  return BMI_FAILURE;
}
static int Get_grid_z(struct Bmi *self, int grid, double *z) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(z);
  return BMI_FAILURE;
}

static int Get_grid_node_count(struct Bmi *self, int grid, int *count) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(count);
  return BMI_FAILURE;
}
static int Get_grid_edge_count(struct Bmi *self, int grid, int *count) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(count);
  return BMI_FAILURE;
}
static int Get_grid_face_count(struct Bmi *self, int grid, int *count) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(count);
  return BMI_FAILURE;
}
static int Get_grid_edge_nodes(struct Bmi *self, int grid, int *edge_nodes) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(edge_nodes);
  return BMI_FAILURE;
}
static int Get_grid_face_edges(struct Bmi *self, int grid, int *face_edges) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(face_edges);
  return BMI_FAILURE;
}
static int Get_grid_face_nodes(struct Bmi *self, int grid, int *face_nodes) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(face_nodes);
  return BMI_FAILURE;
}
static int Get_grid_nodes_per_face(struct Bmi *self, int grid,
                                   int *nodes_per_face) {
  UNUSED(self);
  UNUSED(grid);
  UNUSED(nodes_per_face);
  return BMI_FAILURE;
}
static int Get_value_ptr(struct Bmi *self, const char *name, void **dest_ptr) {
  UNUSED(self);
  UNUSED(name);
  UNUSED(dest_ptr);
  return BMI_FAILURE;
}
static int Get_value_at_indices(struct Bmi *self, const char *name, void *dest,
                                int *inds, int count) {
  UNUSED(self);
  UNUSED(name);
  UNUSED(dest);
  UNUSED(inds);
  UNUSED(count);
  return BMI_FAILURE;
}
static int Set_value_at_indices(struct Bmi *self, const char *name, int *inds,
                                int count, void *src) {
  UNUSED(self);
  UNUSED(name);
  UNUSED(inds);
  UNUSED(count);
  UNUSED(src);
  return BMI_FAILURE;
}

/* -----------------------------------------------------------------------
 * BMI registration
 * -----------------------------------------------------------------------
 */

Bmi *register_bmi_wofost(Bmi *model) {
  if (!model) {
    return model;
  }

  model->initialize = Initialize;
  model->update = Update;
  model->update_until = Update_until;
  model->finalize = Finalize;

  model->get_component_name = Get_component_name;
  model->get_input_item_count = Get_input_item_count;
  model->get_output_item_count = Get_output_item_count;
  model->get_input_var_names = Get_input_var_names;
  model->get_output_var_names = Get_output_var_names;

  model->get_var_grid = Get_var_grid;
  model->get_var_type = Get_var_type;
  model->get_var_itemsize = Get_var_itemsize;
  model->get_var_units = Get_var_units;
  model->get_var_nbytes = Get_var_nbytes;
  model->get_var_location = Get_var_location;

  model->get_current_time = Get_current_time;
  model->get_start_time = Get_start_time;
  model->get_end_time = Get_end_time;
  model->get_time_units = Get_time_units;
  model->get_time_step = Get_time_step;

  model->get_value = Get_value;
  model->get_value_ptr = Get_value_ptr;
  model->get_value_at_indices = Get_value_at_indices;

  model->set_value = Set_value;
  model->set_value_at_indices = Set_value_at_indices;

  model->get_grid_size = Get_grid_size;
  model->get_grid_rank = Get_grid_rank;
  model->get_grid_type = Get_grid_type;

  model->get_grid_shape = Get_grid_shape;
  model->get_grid_spacing = Get_grid_spacing;
  model->get_grid_origin = Get_grid_origin;

  model->get_grid_x = Get_grid_x;
  model->get_grid_y = Get_grid_y;
  model->get_grid_z = Get_grid_z;

  model->get_grid_node_count = Get_grid_node_count;
  model->get_grid_edge_count = Get_grid_edge_count;
  model->get_grid_face_count = Get_grid_face_count;
  model->get_grid_edge_nodes = Get_grid_edge_nodes;
  model->get_grid_face_edges = Get_grid_face_edges;
  model->get_grid_face_nodes = Get_grid_face_nodes;
  model->get_grid_nodes_per_face = Get_grid_nodes_per_face;
  return model;
}
