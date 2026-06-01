import { useEffect, useState } from "react";
import { Monitor, Moon, Sun } from "lucide-react";
import { Link, Route, Routes } from "react-router-dom";
import { DatasetListPage } from "./pages/DatasetListPage";
import { DatasetPage } from "./pages/DatasetPage";

type Theme = "light" | "dark";
type ThemePreference = Theme | "system";

function getInitialThemePreference(): ThemePreference {
  const stored = localStorage.getItem("daq-theme");
  if (stored === "light" || stored === "dark" || stored === "system") {
    return stored;
  }
  return "system";
}

function resolveTheme(preference: ThemePreference): Theme {
  if (preference === "system") {
    return window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light";
  }
  return preference;
}

export default function App() {
  const [themePreference, setThemePreference] = useState<ThemePreference>(getInitialThemePreference);
  const [theme, setTheme] = useState<Theme>(resolveTheme(themePreference));

  useEffect(() => {
    setTheme(resolveTheme(themePreference));
    localStorage.setItem("daq-theme", themePreference);
  }, [themePreference]);

  useEffect(() => {
    document.documentElement.setAttribute("data-theme", theme);
  }, [theme]);

  useEffect(() => {
    if (themePreference !== "system") return;
    const media = window.matchMedia("(prefers-color-scheme: dark)");
    const listener = () => setTheme(media.matches ? "dark" : "light");
    media.addEventListener("change", listener);
    return () => media.removeEventListener("change", listener);
  }, [themePreference]);

  function cycleTheme() {
    setThemePreference((prev) => {
      if (prev === "system") return "light";
      if (prev === "light") return "dark";
      return "system";
    });
  }

  const themeButtonLabel =
    themePreference === "system"
      ? "Theme: System"
      : themePreference === "light"
        ? "Theme: Light"
        : "Theme: Dark";

  const ThemeIcon =
    themePreference === "system" ? Monitor : themePreference === "light" ? Sun : Moon;

  return (
    <div className="mx-auto min-h-screen w-11/12 px-4 py-4 text-text md:w-5/6">
      <header className="mb-4 flex items-center justify-between gap-4 rounded-2xl border border-border bg-panel px-4 py-3 backdrop-blur">
        <Link to="/" className="text-xl font-semibold tracking-tight">
          DAQ CSV Grapher
        </Link>
        <button
          type="button"
          className="inline-flex h-9 w-9 items-center justify-center rounded-full border border-input-border bg-input text-text transition hover:bg-button hover:text-button-text"
          onClick={cycleTheme}
          aria-label={themeButtonLabel}
          title={themeButtonLabel}
        >
          <ThemeIcon size={16} strokeWidth={2} aria-hidden="true" />
        </button>
      </header>
      <Routes>
        <Route path="/" element={<DatasetListPage />} />
        <Route path="/datasets/:slug" element={<DatasetPage theme={theme} />} />
      </Routes>
    </div>
  );
}
