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
