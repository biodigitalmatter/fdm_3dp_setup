# FDM 3DP setup

For LTH Robotlab.

## Python setup

### First install

Install [Anacoda](https://anaconda.org/) if you don't have it already. You can use [miniforge](https://github.com/conda-forge/miniforge#miniforge3) to get the minimal install preconfigured for [conda-forge](https://conda-forge.org/).

Run the Anaconda/Miniconda/Miniforge prompt in this directory and run:

```bash
conda env create --file environment.yml
conda activate fdm_3dp_setup
python -m compas_rhino.install
```

### Update install

```bash
conda env update --file environment.yml --prune
conda activate fdm_3dp_setup
python -m compas_rhino.install
```

## RobotStudio (virtual) setup

See [robotstudio-folder](/robotstudio)

## Docker setup for compas_rrc

Install [Docker Desktop](https://www.docker.com/products/docker-desktop/)

You might need to install and/or update WSL kernel. Follow steps in the Docker
       Desktop installer

### Run containers

Run either the compose file for [virtual
controller](/docker/virtual_controller/compose.yml) or [real controller (serviceport)](/docker/real_controller/compose.yml).

## Slice and run robot

See [slice_and_print.ghx](/slice_and_print.ghx)
