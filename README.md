# DEF2 bicycle speed measurement

This repo contains the code used for an assignment for the TU Delft course Design Engineering for Physicist 2. The main goal of the assignment is to count bicycles and measure their speed on a bike path at the TU Delft campus. The code for the Arduino / ESP8266, the Node-RED instance and the live data dashboard will be hosted here.

Bikes can be detected using conducting plates separated by a flexible material (for example polyethylene foam) that will act as a switch. When a bike rides over the sensor, the switch will close, which can be detected using an interrupt on a microcontroller. The microcontroller used for this project is a Particle Electron.

## Photon
Code for a Particle Photon. The Particle Photon has mainly been used for testing purposes, since the using a cellular connection is quite expensive when compared to Wi-Fi.

## Electron
Photon code ported to a Particle Electron. Using a Particle Electron makes the sensor independent of Wi-Fi, but will make your sensor more expensive.

## Dashboard
Data visualisation using Node-RED, [node-red-contrib-uibuilder](https://flows.nodered.org/node/node-red-contrib-uibuilder) and the Particle Cloud. The main goal of the dashboard is to visualize some basic data like average speed, number of bikes per day, etc. etc.

### Dashboard setup
To complete this setup, some basic knowledge of Node-RED is required. If you've never worked with Node-RED before, it might be useful to first read their [getting started tutorial](https://nodered.org/docs/getting-started/)
- Install [Node-RED](https://node-red.org);
- Install dependencies in Node-RED:
  - [node-red-contrib-particle](https://flows.nodered.org/node/node-red-contrib-particle)
  - [node-red-contrib-uibuilder](https://flows.nodered.org/node/node-red-contrib-uibuilder)
  - [node-red-node-mysql](https://flows.nodered.org/node/node-red-node-mysql). We use MariaDB for data storage, but you can of course use another database. Please note that this might require using different database nodes.
 - Prepare a database. The Node-RED flows store incoming values in this database and use this data for calculations. The flows assume the data table is called `measurement_data`. The table should consist of 3 columns: timestamp (int or bigint), velocity (double) and segment (int).
 - Copy the contents of the [`dashboard/interface`](https://github.com/crnh/def-cycle-speed/tree/master/dashboard/interface) folder to the `.node-red/uibuilder/dashboard/src` folder. On Linux systems, the `.node-red` folder is usually located in `~/.node-red`. On Windows systems this is `%USERPROFILE%/.node-red`.
 - Import the contents of [`dashboard/node-red/flow.json`](https://github.com/crnh/def-cycle-speed/blob/master/dashboard/node-red/flows.json) as a new flow in Node-RED.
 - Set the right credentials in the Particle Cloud nodes and set the database address in the SQLite nodes.
 - Deploy the flows and navigate to `http://<your-node-red-address>/dashboard`. The flow will automatically insert dummy data which should be visible in the dashboard.
