import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  base: '/',
  server: {
    proxy: {
      '/led': {
        target: 'http://192.168.1.227',
        changeOrigin: true,
      },
    },
  },
})
