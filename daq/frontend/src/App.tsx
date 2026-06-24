import { Suspense, lazy, useEffect, useState } from "react";
import { Monitor, Moon, Sun } from "lucide-react";
import { Link, Route, Routes } from "react-router-dom";
import { DatasetListPage } from "./pages/DatasetListPage";
import { Badge, Button, Panel, Tooltip } from "./components/ui";
import rfrDarkLogo from "./assets/rfr-dark-logo.png";
import rfrLightLogo from "./assets/rfr-light-logo.png";

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
  const logoSrc = theme === "dark" ? rfrDarkLogo : rfrLightLogo;

  return (
    <div className="mx-auto min-h-screen w-full max-w-[1540px] px-3 py-3 text-text sm:px-4 lg:px-6">
      <header className="mb-4 flex min-h-16 items-center justify-between gap-4 rounded-lg border border-[var(--header-border)] bg-[var(--header-bg)] px-3 py-2 shadow-sm shadow-black/5 backdrop-blur sm:px-4">
        <Link to="/" className="flex min-w-0 items-center gap-3 font-semibold tracking-tight">
          <img
            src={logoSrc}
            alt="Rutgers Formula Racing"
            className="h-10 w-auto max-w-[190px] object-contain sm:h-12 sm:max-w-[260px]"
          />
          <span className="hidden min-w-0 border-l border-[var(--header-border)] pl-3 sm:block">
            <span className="block truncate text-xs font-bold uppercase tracking-[0.18em] text-subtle">Data Acquisition</span>
            <span className="block truncate text-base font-semibold">CSV Grapher</span>
          </span>
        </Link>
        <div className="flex items-center gap-2">
          <Badge tone="info" className="hidden sm:inline-flex">Telemetry</Badge>
          <Tooltip label={themeButtonLabel}>
            <Button
              type="button"
              variant="ghost"
              size="icon"
              onClick={cycleTheme}
              aria-label={themeButtonLabel}
            >
              <ThemeIcon size={16} strokeWidth={2} aria-hidden="true" />
            </Button>
          </Tooltip>
        </div>
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
