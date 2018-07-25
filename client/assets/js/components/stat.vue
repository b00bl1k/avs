<template>
    <article>
        <p>User: #{{ $route.params.id }}</p>
        <div class="cal-heatmap"></div>
    </article>
</template>

<script>
import axios from 'axios'
import CalHeatMap from 'cal-heatmap'
import moment from 'moment'
import css from 'cal-heatmap/cal-heatmap.css'

var cal = null

export default {
    data () {
        return {
            total: 0,
            keywords: null,
            users: []
        }
    },
    watch: {
        '$route' (to, from) {
            cal.update("https://avs.b00bl1k.ru/api/users/" +
                this.$route.params.id + "/stat")
        }
    },
    mounted () {
        cal = new CalHeatMap()
        cal.init({
            range: 53,
            domain: "week",
            subDomain: "day",
            start: moment().subtract(12, "month").toDate(),
            domainGutter: 1,
            domainLabelFormat: "",
            itemSelector: ".cal-heatmap",
            data: "https://avs.b00bl1k.ru/api/users/" +
                this.$route.params.id + "/stat",
            afterLoadData: this.convert,
            legendColors: {
                min: "#c6e48b",
                max: "#196127",
                empty: "#efefef"
            }
        })
    },
    methods: {
        convert (data) {
            var i, results = {};

            for (i = 0; i < data.length; i++) {
                var timestamp = new Date(data[i].date);
                if (data[i].added > 0) {
                    results[timestamp.getTime() / 1000] = data[i].added;
                }
            }

            return results;
        }
    }
}
</script>