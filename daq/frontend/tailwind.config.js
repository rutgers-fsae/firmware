/** @type {import('tailwindcss').Config} */
export default {
  content: ["./index.html", "./src/**/*.{ts,tsx}"],
  theme: {
    extend: {
      colors: {
        text: "var(--text)",
        muted: "var(--muted)",
        panel: "var(--panel-bg)",
        border: "var(--panel-border)",
        input: "var(--input-bg)",
        "input-border": "var(--input-border)",
        button: "var(--button-bg)",
        "button-hover": "var(--button-bg-hover)",
        "button-text": "var(--button-text)",
        "button-disabled": "var(--button-disabled)",
      },
    },
  },
  plugins: [],
};
