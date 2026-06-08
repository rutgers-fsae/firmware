import { Suspense, lazy, useEffect, useState } from "react";
import { Monitor, Moon, Sun } from "lucide-react";
import { Link, Route, Routes } from "react-router-dom";
import { DatasetListPage } from "./pages/DatasetListPage";
import { Button, Panel } from "./components/ui";

const DatasetPage = lazy(() => import("./pages/DatasetPage").then((mod) => ({ default: mod.DatasetPage })));

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
    <div className="mx-auto min-h-screen w-full max-w-[1500px] px-3 py-3 text-text sm:px-4 lg:px-6">
      <header className="mb-4 flex h-14 items-center justify-between gap-4 border-b border-border">
        <Link to="/" className="flex items-center gap-3 font-semibold tracking-tight">
          <span className="flex h-8 w-8 items-center justify-center rounded-md bg-button text-sm font-bold text-button-text">
            D
          </span>
          <span>DAQ CSV Grapher</span>
        </Link>
        <Button
          type="button"
          variant="ghost"
          size="icon"
          onClick={cycleTheme}
          aria-label={themeButtonLabel}
          title={themeButtonLabel}
        >
          <ThemeIcon size={16} strokeWidth={2} aria-hidden="true" />
        </Button>
      </header>
      <Suspense
        fallback={
          <Panel className="p-4 text-sm text-muted">
            Loading page...
          </Panel>
        }
      >
        <Routes>
          <Route path="/" element={<DatasetListPage />} />
          <Route path="/datasets/:slug" element={<DatasetPage theme={theme} />} />
        </Routes>
      </Suspense>
    </div>
  );
}
