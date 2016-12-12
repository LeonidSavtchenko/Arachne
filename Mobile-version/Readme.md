# ARACHNE for Mobile OS #

"Mobile-version" contains the host version developed for [Matlab mobile](https://www.mathworks.com/products/matlab-mobile.html). You can use a mobile device operating under Android or iOS to run the host on your desktop computer. Currently you cannot run the host on the preinstalled cluster 144.82.46.83 from a mobile device, but you still can run the worker on it.

Matlab mobile does not support interaction with user during execution of M-code in remote desktop Matlab. Therefore, you cannot use GUI normally provided by the host to adjust any parameters. The simulation will be conducted with all default GUI parameters.

There are 4 special entry points in the application for mobile mode:

* START_MOBILE_Arachne
* SCRIPT_MOBILE_TakeSnapshot
* UTILITY_MOBILE_ComputeMaxModelSize
* UTILITY_MOBILE_PlotStdpModels

The next 4 entry points are suitable for both mobile and desktop modes:

* SCRIPT_RecoverBackupProgress
* SCRIPT_TerminateBackgroundProcess
* SCRIPT_KillBackgroundProcess
* SCRIPT_CleanUp

When you call a mobile entry point, you need to pass a special struct as an argument to give the predefined answers for any questions the program can ask and the predefined choices for any menus the program can provide. The structure can have the next fields:

* 'WhatToDo' -- the answer to the question about the simulation scenario. The supported values are: 1, 2 and 3.
* 'AdditionalPeriod' -- the additional period to simulate, which can be given in milliseconds (e.g. '1234.5 ms') or iterations (e.g. '12345 it').
* 'ContinueWithBadParams' -- whether to continue the simulation even though the input parameters are so that bad accuracy will be provided. The supported values are: 'Yes' and 'No'.
* 'MemorizeOrRecall' -- whether to memorize or recall the image in this simulation session. The supported values are: 'Memorize' and 'Recall'.
* 'ImageFile' -- path to the image BMP file in black and white colors that will be used as a vector of excitation for e-neurons.

## Examples ##

Starting simulation from scratch with Matlab mobile:


```
#!matlab

choices = struct('WhatToDo', 1, 'ImageFile', 'D:\excitation\image.bmp');
START_MOBILE_Arachne(choices);
```


Continuing the previous simulation session with Matlab mobile:


```
#!matlab

choices = struct('WhatToDo', 3, 'AdditionalPeriod', '10000 it', 'MemorizeOrRecall', 'Recall', 'ImageFile', 'D:\excitation\image.bmp');
START_MOBILE_Arachne(choices);
```
