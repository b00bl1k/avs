function converter(response)
{
    var i, results = {};

    for (i = 0; i < response.data.length; i++) {
        var timestamp = new Date(response.data[i].date);
        if (response.data[i].count > 0) {
            results[timestamp.getTime() / 1000] = response.data[i].count;
        }
    }

    return results;
}

function modifyProfilePage()
{

    var re = /.*?(\d+)$/;
    var match = re.exec(window.location.pathname);

    var profile = document.getElementsByClassName('profile');
    var block = profile[0].getElementsByClassName('service-block-v3');
    var counter = block[0].getElementsByClassName('counter');
    var counter_text = counter[0].innerText;

    user_id = match[1];
    block[0].classList.remove('service-block-grey');
    block[0].innerHTML = '<p><a href="//avto-nomer.ru/gallery.php?usr=' + user_id +
        '">Галерея / ' + counter_text + '</a></p><div id="cal-heatmap"></div>';

    var cal = new CalHeatMap();
    var prevYear = new Date();
    // prevYear.setFullYear(prevYear.getFullYear() - 1);
    prevYear.setDate(prevYear.getDate() - 52 * 7);

    cal.init({
        range: 53,
        domain: "week",
        subDomain: "day",
        start: prevYear,
        domainGutter: 1,
        domainLabelFormat: "",
        data: "https://avs.b00bl1k.ru/api/users/" + user_id + "/stat",
        afterLoadData: converter,
        legendColors: {
            min: "#c6e48b",
            max: "#196127",
            empty: "#efefef"
        }
    });
}

var path = window.location.pathname;

if (path.startsWith('/user')) {
    modifyProfilePage();
}