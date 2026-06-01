import { useEffect, useState } from "react";
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

  const themeIcon =
    themePreference === "system" ? "◐" : themePreference === "light" ? "☀" : "☾";

  return (
    <div className="app-shell">
      <header className="app-header">
        <Link to="/">DAQ CSV Grapher</Link>
        <button
          type="button"
          className="theme-toggle"
          onClick={cycleTheme}
          aria-label={themeButtonLabel}
          title={themeButtonLabel}
        >
          <span aria-hidden="true" className="theme-icon">{themeIcon}</span>
        </button>
      </header>
      <Routes>
        <Route path="/" element={<DatasetListPage />} />
        <Route path="/datasets/:slug" element={<DatasetPage theme={theme} />} />
      </Routes>
    </div>
  );
}
