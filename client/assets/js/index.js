import Vue from 'vue'
import VueRouter from 'vue-router'
import Application from './components/application.vue'
import Main from './components/main.vue'
import axios from 'axios'

Vue.use(VueRouter)
Vue.component('application', Application)
Vue.component('main', Main)

axios.defaults.baseURL = 'https://avs.b00bl1k.ru';

const router = new VueRouter({
    routes: [
        {path: '/', component: Main},
    ]
})

var app = new Vue({
    el: '#app',
    router,
    template: '<application />'
})
