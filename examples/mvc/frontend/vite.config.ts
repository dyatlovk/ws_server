import react from '@vitejs/plugin-react'
import { resolve } from 'path'
import { defineConfig } from 'vite'

export default defineConfig({
  base: '/',
  plugins: [react()],
  server: {
    host: '127.0.0.1',
    port: 3000,
  },
  css: {
    preprocessorOptions: {
      scss: {},
    },
  },
  resolve: {
    alias: {
      // Use import.meta.url to get the current module URL
      '@': new URL('src', import.meta.url).pathname,
      '@components': new URL('src/components', import.meta.url).pathname,
      '@layouts': new URL('src/layouts', import.meta.url).pathname,
      // Add other aliases as needed
    },
  },
  build: {
    target: 'esnext',
    rollupOptions: {
      input: {
        main: resolve(__dirname, 'index.html'),
      },
    },
  },
  test: {
    environment: 'jsdom',
    includeSource: ['src/**/*.{js,ts,tsx}'],
  },
})
