# Basic c++ node addon using NAPI

This example is to help explain debugging in VSCode for C++ based NodeJS addons.



#Requriements:
a) Install lldb extension to debug in vs code.

b) Node v10

#To Run

1. `git clone this repo`
2. `npm install`
3. `npm run rebuild:dev`
4. `npm run start`

#To debug add following in visual studio code project launch.json

{
    
    "version": "0.2.0",
    "configurations": [
        
        {
            "type": "lldb",
            "request": "launch",
            "name": "Debug",
            "preLaunchTask": "npm: build:dev",
            "program": "C:/Program Files/nodejs/node",
            "args": ["E:/experiment-projects/basic-node-addon/index.js"]
        }
    ]
}

Note: Here program is path for node js exe and args is path of index.js 
# cpp-node-addon
