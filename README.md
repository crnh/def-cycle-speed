# DEF2 bicycle speed measurement

This repo contains the code used for an assignment for the TU Delft course Design Engineering for Physicist 2. The main goal of the assignment is to count bicycles and measure their speed on a bike path at the TU Delft campus. The code for the Arduino / ESP8266, the Node-RED instance and the live data dashboard will be hosted here.

## Arduino
Arduino code for the sensor node. Only used for testing.

## Photon
Particle Photon version of the Arduino code, will be used for the sensor node.

## Dashboard
Data visualisation using Node-RED, [node-red-contrib-uibuilder](https://flows.nodered.org/node/node-red-contrib-uibuilder) and the Particle Cloud. The main goal of the dashboard is to visualize some basic data like average speed, number of bikes per day, etc. etc.

### Dashboard setup
To complete this setup, some basic knowledge of Node-RED is required. If you've never worked with Node-RED before, it might be useful to first read their [getting started tutorial](https://nodered.org/docs/getting-started/)
- Install [Node-RED](https://node-red.org);
- Install dependencies in Node-RED:
  - [node-red-contrib-particle](https://flows.nodered.org/node/node-red-contrib-particle)
  - [node-red-contrib-uibuilder](https://flows.nodered.org/node/node-red-contrib-uibuilder)
  - [node-red-node-sqlite](https://flows.nodered.org/node/node-red-node-sqlite) (We use SQLite for data storage, but you can of course use another database).
 - Prepare a database. The Node-RED flows store incoming values in this database and use this data for calculations.
 - Copy the contents of the [`dashboard/interface`](https://github.com/crnh/def-cycle-speed/tree/master/dashboard/interface) folder to the `.node-red/uibuilder/dashboard/src` folder. On Linux systems, the `.node-red` folder is usually located in `~/.node-red`. On Windows systems this is `%USERPROFILE%/.node-red`.
 - Import the contents of [`dashboard/node-red/flow.json`](https://github.com/crnh/def-cycle-speed/blob/master/dashboard/node-red/flows.json) as a new flow in Node-RED.
 - Set the right credentials in the Particle Cloud nodes and set the database address in the SQLite nodes.
 - Deploy the flows and navigate to `http://<your-node-red-address>/dashboard`. The flow will automatically insert dummy data which should be visible in the dashboard.
