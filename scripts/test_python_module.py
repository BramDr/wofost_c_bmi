import pathlib as pl
import datetime as dt
import pymt_wofost

config_file = pl.Path("ci/configuration.ini")

model = pymt_wofost.WOFOST()
component_name = model.get_component_name()

print(f"Initializing {component_name} with configuration file {config_file}")
model.initialize(str(config_file))

start_time = model.get_start_time()
end_time = model.get_end_time()
time_step = model.get_time_step()

print(f"Running {component_name} from time {start_time} to {end_time} with time step {time_step}")

steps = int((end_time - start_time) // time_step)
for t in range(steps):
    date = dt.datetime.fromtimestamp(start_time + t * time_step, tz=dt.timezone.utc)
    print(f"Updating {component_name} at time {date.year:04d}-{date.month:02d}-{date.day:02d}")
    model.update()

print(f"Finalizing {component_name}")
model.finalize()