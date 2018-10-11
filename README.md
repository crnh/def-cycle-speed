# DEF2 bicycle speed measurement

This repo contains the code used for an assignment for the TU Delft course Design Engineering for Physicist 2. The main goal of the assignment is to count bicycles and measure their speed on a bike path at the TU Delft campus. The code for the Arduino / ESP8266, the Node-RED instance and the live data dashboard will be hosted here.

## Arduino / ESP
Arduino code for the sensor node.

## Dashboard
An unofficial sketch for the live data dashboard. The dashboard should eventually integrate with Node-RED, using [node-red-contrib-uibuilder](https://flows.nodered.org/node/node-red-contrib-uibuilder). The main goal of the dashboard is to visualize some basic data like average speed, number of bikes per day, etc. etc. Data visualisation will be accomplished using [chart.js](https://www.chartjs.org/).
