import { defineConfig } from 'vite';

export default defineConfig({
  root: 'web',
  base: './',
  // Maintainers can preview a new UI against a bench unit without embedding or
  // flashing it: ADVATEK_DEVICE_URL=http://192.168.1.100 pnpm dev.
  server: process.env.ADVATEK_DEVICE_URL ? {
    proxy: {'/api': {target: process.env.ADVATEK_DEVICE_URL, changeOrigin: true}},
  } : undefined,
  build: {
    outDir: '../build/web',
    emptyOutDir: true,
    cssCodeSplit: false,
    minify: 'esbuild',
    target: 'es2020',
    rollupOptions: {
      output: {
        entryFileNames: 'app.js',
        assetFileNames: 'app.[ext]',
      },
    },
  },
});
