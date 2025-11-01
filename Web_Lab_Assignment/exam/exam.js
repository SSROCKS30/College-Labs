var data = [];
function acceptData(){
    const tripID = document.getElementById('tid').value;
    const userID = document.getElementById('uid').value;
    const from = document.getElementById('from').value;
    const to = document.getElementById('to').value;
    const date = document.getElementById('date').value;
    const TransportType = document.getElementById('ttype').value;
    const Fare = document.getElementById('fare').value;

    const trip = {
        tripID: tripID,
        userID: userID,
        from: from,
        to: to,
        date: date,
        transportType: TransportType,
        fare: Fare
    };

    data.push(trip);

    console.log(data);
}

function fetch(sdate, edate){
    return function(data){
        return data.filter(function(trip) {
            return trip.date >= sdate && trip.date <= edate;
        });
    };
}

function show(){
    const startdate = document.getElementById('sdate').value;
    const enddate = document.getElementById('edate').value;

    var fetchBydate = fetch(startdate, enddate);
    var result = fetchBydate(data);

    console.log(result);
}