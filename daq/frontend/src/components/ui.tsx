import type { ButtonHTMLAttributes, HTMLAttributes, InputHTMLAttributes, SelectHTMLAttributes } from "react";
import { cxClasses } from "./ui-utils";

type Tone = "default" | "danger" | "warning" | "info" | "success";

export function Panel({ className, ...props }: HTMLAttributes<HTMLElement>) {
  return (
    <section
      className={cxClasses("rounded-lg border border-border bg-panel shadow-sm", className)}
      {...props}
    />
  );
}

type ButtonProps = ButtonHTMLAttributes<HTMLButtonElement> & {
  variant?: "primary" | "secondary" | "ghost" | "danger";
  size?: "sm" | "md" | "icon";
};

export function Button({ className, variant = "secondary", size = "md", ...props }: ButtonProps) {
  const variantClass = {
    primary: "border-transparent bg-button text-button-text hover:bg-button-hover",
    secondary: "border-input-border bg-input text-text hover:border-button hover:text-text",
    ghost: "border-transparent bg-transparent text-muted hover:bg-[var(--surface-soft)] hover:text-text",
    danger: "border-transparent bg-[var(--danger-soft)] text-[var(--danger)] hover:border-[var(--danger)]",
  }[variant];
  const sizeClass = {
    sm: "h-8 px-2.5 text-xs",
    md: "h-9 px-3 text-sm",
    icon: "h-9 w-9 p-0",
  }[size];

  return (
    <button
      className={cxClasses(
        "inline-flex items-center justify-center gap-2 rounded-md border font-medium transition focus:outline-none focus:ring-2 focus:ring-[var(--focus)] focus:ring-offset-2 focus:ring-offset-[var(--bg)] disabled:cursor-not-allowed disabled:opacity-50",
        variantClass,
        sizeClass,
        className,
      )}
      {...props}
    />
  );
}

export function FieldInput({ className, ...props }: InputHTMLAttributes<HTMLInputElement>) {
  return (
    <input
      className={cxClasses(
        "h-9 rounded-md border border-input-border bg-input px-3 text-sm text-text outline-none transition placeholder:text-subtle focus:border-button focus:ring-2 focus:ring-[var(--focus)]/30",
        className,
      )}
      {...props}
    />
  );
}

export function FieldSelect({ className, ...props }: SelectHTMLAttributes<HTMLSelectElement>) {
  return (
    <select
      className={cxClasses(
        "h-9 rounded-md border border-input-border bg-input px-3 text-sm text-text outline-none transition focus:border-button focus:ring-2 focus:ring-[var(--focus)]/30",
        className,
      )}
      {...props}
    />
  );
}

export function Alert({
  className,
  tone = "default",
  ...props
}: HTMLAttributes<HTMLParagraphElement> & { tone?: Tone }) {
  const toneClass = {
    default: "border-border bg-[var(--surface-soft)] text-muted",
    danger: "border-[color-mix(in_srgb,var(--danger)_35%,transparent)] bg-[var(--danger-soft)] text-[var(--danger)]",
    warning: "border-[color-mix(in_srgb,var(--warning)_35%,transparent)] bg-[var(--warning-soft)] text-[var(--warning)]",
    info: "border-[color-mix(in_srgb,var(--info)_35%,transparent)] bg-[var(--info-soft)] text-[var(--info)]",
    success: "border-[color-mix(in_srgb,var(--success)_35%,transparent)] bg-[var(--success-soft)] text-[var(--success)]",
  }[tone];

  return (
    <p
      className={cxClasses("rounded-md border px-3 py-2 text-sm", toneClass, className)}
      {...props}
    />
  );
}
