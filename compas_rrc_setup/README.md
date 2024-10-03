# FDM 3DP setup using compas_rrc

For LTH Robotlab.

## compas_rrc setup

### First install

Install [miniforge](https://github.com/conda-forge/miniforge#miniforge3) if you don't have it already.

Open the forge prompt in this directory and run:

```bash
conda env create --file environment.yml
conda activate fdm_3dp_setup
python -m compas_rhino.install
```

### Update install

```bash
conda env update --file environment.yml
conda activate fdm_3dp_setup
python -m compas_rhino.install
```

## RobotStudio (virtual) setup

See [robotstudio-folder](/robotstudio)

## Docker setup for compas_rrc

Install [Docker Desktop](https://www.docker.com/products/docker-desktop/)

You might need to install and/or update WSL kernel. Follow steps in the Docker
Desktop installer.

### Run containers

Run either the compose file for [virtual
controller](/docker/virtual_controller/compose.yml) or [real controller
(serviceport)](/docker/real_controller/compose.yml).

## Slice and run robot

See [slice_and_print.ghx](/slice_and_print.ghx)
