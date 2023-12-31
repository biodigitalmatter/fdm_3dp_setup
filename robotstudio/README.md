# ABB Robotstudio setup

## Install ABB Robotstudio

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

## Setting up robot station

There's a [prepared "pack-and-go" file that you can "unpack-and-work" with in
this directory](V_LTH2400_RW612.rspag).

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

## Footnotes

[^footnote-robotstudio-check-out]:
    You can check out a license for up to 90
    days. This allows you to run RobotStudio even when offline. You can check
    out a license using the Activation Wizard found under `File > Options > Licensing`.
