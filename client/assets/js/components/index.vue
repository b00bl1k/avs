<template>
    <article>
        <p>Поиск по имени: <input v-model="keywords" type="text"></p>
        <table>
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Имя</th>
                </tr>
            </thead>
            <tbody>
                <tr v-for="user in users" :key="user.id">
                    <td>{{ user.id }}</td>
                    <td>
                        <router-link :to="{ name: 'Stat', params: { id: user.id }}">
                            {{ user.name }}
                        </router-link>
                    </td>
                </tr>
            </tbody>
        </table>
    </article>
</template>

<script>
import axios from 'axios'

export default {
    data () {
        return {
            total: 0,
            keywords: null,
            users: []
        }
    },
    created () {
        this.fetch()
    },
    watch: {
        keywords (after, before) {
            this.fetch()
        }
    },
    methods: {
        fetch () {
            var params = { params: { q: this.keywords, limit: 10 } }
            axios.get('/api/users', params)
                .then(response => {
                    this.total = response.data.total
                    this.users = response.data.result
                })
                .catch(e => {
                })
        }
    }
}
</script>
