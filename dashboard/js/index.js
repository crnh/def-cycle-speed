// constants
const unitTexts = { // text to use in the UI for the given unit
    kmh: 'km/h',
    ms: 'm/s',
    lytp: 'ly/t<sub>p</sub>'
}

const unitScales = { // factor for scaling the provided value to a given unit
    kmh: 3.6,
    ms: 1,
    lytp: 1.7549119778484140233722871452421e59
}

const lastSpeedId = '#last-speed-';
const lastTimeId = '#last-time-';

// global variables
let startDate, endDate, unit, unitScale;
let bikeCounter = 0;
setUnit('kmh', true);

toastr.options = {
    "closeButton": false,
    "debug": false,
    "newestOnTop": true,
    "progressBar": false,
    "positionClass": "toast-top-right",
    "preventDuplicates": false,
    "onclick": null,
    "showDuration": "300",
    "hideDuration": "1000",
    "timeOut": "5000",
    "extendedTimeOut": "1000",
    "showEasing": "swing",
    "hideEasing": "linear",
    "showMethod": "fadeIn",
    "hideMethod": "fadeOut"
}

$(document).ready(function () {
    // Turn on debugging (default is off)
    uibuilder.debug(true)

    // If msg changes - msg is updated when a standard msg is received from Node-RED over Socket.IO
    // Note that you can also listen for 'msgsReceived' as they are updated at the same time
    // but newVal relates to the attribute being listened to.
    uibuilder.onChange('msg', function (msg) {
        console.info('property msg changed!')
        console.dir(msg)

        switch (msg.topic) {
            case 'bikeDetected':
                bikeDetected(msg.payload);
        }

        if (msg.topic == 'mqttValue') {
            $('#showMsg').text(msg.payload);
        }
        //uibuilder.set('msgCopy', newVal)
    })

}) // --- End of JQuery Ready --- //

function updatePreferences() {
    let startDateSetting = document.getElementById('graph-start-date').value;
    let startTimeSetting = document.getElementById('graph-start-time').value;

    let endDateSetting = document.getElementById('graph-end-date').value;
    let endTimeSetting = document.getElementById('graph-end-time').value;

    let unitSetting = document.getElementById('unit-select').value;

    console.log(startDateSetting, startTimeSetting);
    startDate = new Date(startDateSetting + startTimeSetting);
    startDate = new Date(endDateSetting + endTimeSetting);
    setUnit(unitSetting);
}

function setUnit(u, setFirstTime = false) {
    $('.speed-unit').html(unitTexts[u]);
    // let velocityInMperS = scaleVelocity(document.getElementById('last-speed').value, 'ms');
    // $('#last-speed').text(scaleVelocity(velocityInMperS, u));

    // if (!setFirstTime) {
    //     for (let i = 0; i < 4; i++) {
    //         let lastSpeedInMs = scaleVelocity(Number(document.getElementById('last-speed-' + i).value), 'ms');
    //         $(lastSpeedId + i).text(scaleVelocity(lastSpeedInMs, u))
    //     }
    // }

    unit = u;
}

function scaleVelocity(v, u) {
    v = Number(v);
    if (u == 'lytp') {
        return (v * unitScales[u] / 1e59).toFixed(1) + 'e59';
    } else {
        return (v * unitScales[u]).toFixed(1);
    }
}

function bikeDetected(payload) {
    for (let i = 0; i < payload.length; i++) {
        let segment = payload[i].segment;
        let time = new Date(Number(payload[i].timestamp * 1000));
        let velocity = scaleVelocity(payload[i].velocity, unit);

        $(lastSpeedId + segment).text(velocity);
        $(lastTimeId + segment).text(`${time.getHours()}:${time.getMinutes()}:${time.getSeconds()}`);
    }

    // Change UI elements
    bikeCounter += payload.length;
    $('#counter').text(bikeCounter);
}