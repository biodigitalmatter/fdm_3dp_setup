# Setup

## Work and tool objects

Set up work object based on the print surface.

Save work objects and tool objects to the USER module so they don't get overwritten.

## RAPID program setup

### Load programs on robot controller

After saving a program from <a href="https://raw.githubusercontent.com/biodigitalmatter/fdm_3dp_setup/refs/heads/main/rapid_program_setup/planes_to_3dp_rapid_program.ghx" download>planes_to_3dp_rapid_program.ghx</a> you will end up with two files. One called "your_filename.mod" and one called "your_filename.pgf".

To run these on the robot:

1. Copy .mod and .pgf to a USB stick
1. Plug in the USB stick into the FlexPendant
1. Go to `Production Window`
1. Click `Load program` in bottom left corner.
1. Navigate to the USB stick (press the directory up button until you see `/hddn` and `/usbn` where `n` is number of units)
1. Click on the entry for the USB drive and select "your_filename.pgf"
1. After some popups, press `PP to main`
1. Run the program!

## ABB Robotstudio setup for tests and simulation

### Install ABB Robotstudio

[Download installer for ABB RobotStudio](https://new.abb.com/products/robotics/robotstudio)

You need to provide personal information to download the software.

Extract the zip file after the download has finished and run setup.exe in the
RobotStudio folder.

During setup when asked for license, either use the trial (30 days) or the
license server at LTH (<robotstudio.cs.lth.se>). If you are not at LTH you will
need to use [LTH's
VPN](https://luservicedesk.service-now.com/support_en?id=kb_article_en&sys_id=3fcba671db7c6c506452cd4d0b96198c).

If you use the license server at LTH you will need to have access to the license
server whenever you run Robotstudio, unless you "check-out" a license[^footnote-robotstudio-check-out].

### Setting up robot station

There's a [prepared "pack-and-go" file that you can "unpack-and-work" with in
this directory](`V_LTH2400_RW612_compas_rrc.rspag`).

Before unpacking though you will need to make sure that you have the correct
RobotWare version installed.

You will find RobotWare version 6 and 7 in `Add-ins > Gallery`. It's usually the
first two ones. Select "Robotware for IRC5". In side panel to the right select
`6.12.04` and click `Add`.

After you have installed this you can go ahead and open the pack-and-go file. Go
to `File > Share > Unpack and work`.

After clicking through the wizard and waiting a bit you'll see "Controller
status: 1/1" in green in the bottom right controller.

You can find manuals and tutorials under `File > Help`. Both the RobotStudio
help here as well as the manuals for IRC5 are helpful.

### Important for non-compas_rrc users!

Disable all other tasks than T_ROB1!

In RobotStudio, uncheck everything else in Controller > Selected Tasks.

On the FlexPendant, click the cog wheels in the bottom right and select the first icon with a circle and three nodes. Uncheck everything else.

### Steps to simulate

1. Generate RAPID-code using [provided grasshopper file](../grasshopper/planes_to_3dp_rapid_program.ghx)
1. Start solution created from [pack-and-go file](./V_LTH2400_RW612_compas_rrc.rspag). You only need to "unpack and work" once.
1. In the `RAPID` view in the controller tree to the left:
    1. Expand `RAPID`
    1. Right-click `T_ROB1` and select `Load program`. Select your program file.
    1. Click through the prompts.
1. Click `Controller > FlexPendant` ribbon.
1. Open the production view.
1. Click `PP to main`
1. Press play to run the program.

#### Troubleshooting

* Slow? Open Options, go to Simulation Clock and check "As fast as possible".

### Footnotes

[^footnote-robotstudio-check-out]:
    You can check out a license for up to 90
    days. This allows you to run RobotStudio even when offline. You can check
    out a license using the Activation Wizard found under `File > Options > Licensing`.
