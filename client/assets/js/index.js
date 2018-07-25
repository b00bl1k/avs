import Vue from 'vue'
import VueRouter from 'vue-router'
import Application from './components/application.vue'
import Index from './components/index.vue'
import Stat from './components/stat.vue'
import axios from 'axios'

Vue.use(VueRouter)
Vue.component('application', Application)
Vue.component('index', Index)
Vue.component('stat', Stat)

axios.defaults.baseURL = 'https://avs.b00bl1k.ru';

const router = new VueRouter({
    routes: [
        {path: '/', name: "Index", component: Index},
        {path: '/user/:id', name:"Stat", component: Stat}
    ]
})

var app = new Vue({
    el: '#app',
    router,
    template: '<application />'
})
