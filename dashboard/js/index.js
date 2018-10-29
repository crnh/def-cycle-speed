// constants
const unitTexts = { // text to use in the UI for the given unit
    kmh: 'km/h',
    ms: 'm/s',
    lytp: 'ly/t<sub>p</sub>'
}

const unitScales = { // factor for scaling the provided value to a given unit
    kmh: 3.6,
    ms: 1,
    lytp: 5.69846061636014590333e-60
}

const lastSpeedId = '#last-speed-';
const lastTimeId = '#last-time-';
const dataTableId = '#data-table'

// global variables
let unit, unitScale;
let bikeCounter = 0;
let startDate = getCookie('startdate') ? new Date(getCookie('startdate')) : null;
let endDate = getCookie('enddate') ? new Date(getCookie('enddate')) : null;
setUnit(getCookie('unit') || 'kmh', true);

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
    let sparkline0 = new Sparkline('sparkline0');
    let sparkline1 = new Sparkline('sparkline1');
    let sparkline2 = new Sparkline('sparkline2');
    let sparkline3 = new Sparkline('sparkline3');

    // Turn on debugging (default is off)
    //uibuilder.debug(true)

    // If msg changes - msg is updated when a standard msg is received from Node-RED over Socket.IO
    // Note that you can also listen for 'msgsReceived' as they are updated at the same time
    // but newVal relates to the attribute being listened to.
    uibuilder.onChange('msg', function (msg) {
        // console.info('property msg changed!')
        // console.dir(msg)

        switch (msg.topic) {
            case 'bikeDetected':
                bikeDetected(msg.payload);
                break;
            case 'queryResult':
                queryToTable(msg.payload);
                break;
        }

        if (msg.topic == 'mqttValue') {
            $('#showMsg').text(msg.payload);
        }
    })

    function scaleVelocity(v, u) {
        v = Number(v);
        if (u == 'lytp') {
            return (v * unitScales[u] / 1e-60).toFixed(1) + 'e-60';
        } else {
            return (v * unitScales[u]).toFixed(1);
        }
    }

    function bikeDetected(payload) {
        if (payload.length == 0) toastr["info"]("length == 0")

        for (let i = 0; i < payload.length; i++) {
            let segment = payload[i].segment;
            let time = new Date(Number(payload[i].timestamp * 1000));
            let velocity = Number(payload[i].velocity);
            let scaledVelocity = scaleVelocity(velocity, unit);
            let absoluteVelocity = Math.abs(velocity);

            $(lastSpeedId + segment).text(scaledVelocity);
            $(lastTimeId + segment).text(`${time.getHours()}:${time.getMinutes()}:${time.getSeconds()}`);

            switch (segment) {
                case '0':
                    sparkline0.update(absoluteVelocity);
                    break;
                case '1':
                    sparkline1.update(absoluteVelocity);
                    break;
                case '2':
                    sparkline2.update(absoluteVelocity);
                    break;
                case '3':
                    sparkline3.update(absoluteVelocity);
                    break;
            }
        }

        // Change UI elements
        bikeCounter += payload.length;
        $('#counter').text(bikeCounter);
    }

    function queryToTable(payload) {
        if (payload.length == 0) {
            toastr["info"]('Your query didn\'t return any data');
        } else {
            $(dataTableId).html('');
            for (let i = 0; i < payload.length; i++) {
                let dataset = payload[i];
                let date = new Date(+dataset.timestamp * 1000);
                let tableString = `<tr><td>${date.getDate()}-${date.getMonth()}-${date.getFullYear()}</td><td>${date.getHours()}:${date.getMinutes()}:${date.getSeconds()}</td><td>${dataset.velocity}</td><td>${dataset.segment}</td></tr>`

                $(dataTableId).append(tableString);
            }
        }
    }

}) // --- End of JQuery Ready --- //

function Sparkline(elementId) {
    this.ctx = document.getElementById(elementId);
    this.chart = new Chart(this.ctx, {
        type: 'line',
        data: {
            labels: ['', '', '', '', ''],
            datasets: [{
                data: [0, 0, 0, 0, 0],
                backgroundColor: '#007bff',
                borderColor: '#007bff',
                borderWidth: 1
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            elements: {
                point: {
                    radius: 0
                }
            },
            scales: {
                xAxes: [{
                    display: false
                }],
                yAxes: [{
                    display: false,
                    ticks: {
                        beginAtZero: true
                    }
                }]
            },
            legend: {
                display: false
            },
            tooltips: {
                enabled: false
            },
            animation: {
                easing: 'easeOutQuint'
            }
        }
    });

    this.update = function (newValue) {
        this.chart.data.datasets[0].data.push(newValue);
        this.chart.data.labels.push('');
        this.chart.data.labels.shift();
        this.chart.data.datasets.forEach(dataset => {
            dataset.data[0] = dataset.data[1]
            dataset.data.splice(1, 1)
        })
        this.chart.update();
    }
}

function updatePreferences() {
    let startDateSetting = document.getElementById('graph-start-date').value;
    let startTimeSetting = document.getElementById('graph-start-time').value;

    let endDateSetting = document.getElementById('graph-end-date').value;
    let endTimeSetting = document.getElementById('graph-end-time').value;

    let unitSetting = document.getElementById('unit-select').value;

    console.log(startDateSetting, startTimeSetting);
    startDate = new Date(startDateSetting + ' ' + startTimeSetting);
    endDate = new Date(endDateSetting + ' ' + endTimeSetting);
    setUnit(unitSetting);

    setCookie('startdate', startDate);
    setCookie('enddate', endDate);
    setCookie('unit', unit);
}

function sendQuery() {
    if (Number(endDate) < Number(startDate)) {
        toastr['error']('Query failed: date from is later than date to');
    } else if (!endDate || !startDate) {
        toastr['error']('Query failed: please specify a query period in the settings menu');
    } else {
        uibuilder.send({
            'topic': 'queryDatabase',
            'payload': {
                'startDate': Number(startDate) / 1000,
                'endDate': Number(endDate) / 1000
            }
        });
    }
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


function setCookie(cname, cvalue) {
    document.cookie = cname + '=' + cvalue;
}

function getCookie(cname) {
    var name = cname + "=";
    var ca = document.cookie.split(';');
    for (var i = 0; i < ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return null;
}