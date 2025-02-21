import { defineConfig } from "vite";
import { fileURLToPath } from "node:url";
import legacy from "@vitejs/plugin-legacy";
import { dirname, resolve } from "node:path";

const __dirname = dirname(fileURLToPath(import.meta.url));

export default defineConfig({
    build: {
        rollupOptions: {
            input: {
                main: resolve(__dirname, "index.html"),
                settings: resolve(__dirname, "settings.html")
            }
        },
    },
	plugins: [legacy({
        targets: ['defaults', 'not IE 11', 'iOS 12'], 
        additionalLegacyPolyfills: ['regenerator-runtime/runtime']
    })],
});
