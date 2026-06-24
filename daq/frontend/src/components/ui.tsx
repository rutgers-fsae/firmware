import type {
  ButtonHTMLAttributes,
  HTMLAttributes,
  InputHTMLAttributes,
  LabelHTMLAttributes,
  ReactElement,
  SelectHTMLAttributes,
} from "react";
import { cn } from "@/lib/utils";

type Tone = "default" | "danger" | "warning" | "info" | "success";

export function Panel({ className, ...props }: HTMLAttributes<HTMLElement>) {
  return (
    <section
      className={cn("rounded-lg border border-border bg-panel shadow-sm shadow-black/5 backdrop-blur", className)}
      {...props}
    />
  );
}

export function Card({ className, ...props }: HTMLAttributes<HTMLElement>) {
  return <article className={cn("rounded-lg border border-border bg-panel shadow-sm shadow-black/5", className)} {...props} />;
}

export function CardHeader({ className, ...props }: HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("flex items-start justify-between gap-3 border-b border-border px-4 py-3", className)} {...props} />;
}

export function CardContent({ className, ...props }: HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("p-4", className)} {...props} />;
}

type ButtonProps = ButtonHTMLAttributes<HTMLButtonElement> & {
  variant?: "primary" | "secondary" | "ghost" | "danger" | "outline";
  size?: "sm" | "md" | "icon";
};

export function Button({ className, variant = "secondary", size = "md", ...props }: ButtonProps) {
  const variantClass = {
    primary: "border-transparent bg-button text-button-text shadow-sm hover:bg-button-hover",
    secondary: "border-input-border bg-input text-text shadow-sm hover:border-button hover:bg-surface-soft",
    outline: "border-input-border bg-transparent text-text hover:border-button hover:bg-surface-soft",
    ghost: "border-transparent bg-transparent text-muted hover:bg-surface-soft hover:text-text",
    danger: "border-transparent bg-[var(--danger-soft)] text-[var(--danger)] hover:border-[var(--danger)]",
  }[variant];
  const sizeClass = {
    sm: "h-8 px-2.5 text-xs",
    md: "h-9 px-3 text-sm",
    icon: "h-9 w-9 p-0",
  }[size];

  return (
    <button
      className={cn(
        "inline-flex shrink-0 items-center justify-center gap-2 rounded-md border font-medium transition focus:outline-none focus:ring-2 focus:ring-ring/40 focus:ring-offset-2 focus:ring-offset-[var(--bg)] disabled:pointer-events-none disabled:opacity-50",
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
      className={cn(
        "h-9 rounded-md border border-input-border bg-input px-3 text-sm text-text shadow-sm outline-none transition placeholder:text-subtle focus:border-button focus:ring-2 focus:ring-ring/30 disabled:cursor-not-allowed disabled:opacity-50",
        className,
      )}
      {...props}
    />
  );
}

export function FieldSelect({ className, ...props }: SelectHTMLAttributes<HTMLSelectElement>) {
  return (
    <select
      className={cn(
        "h-9 rounded-md border border-input-border bg-input px-3 text-sm text-text shadow-sm outline-none transition focus:border-button focus:ring-2 focus:ring-ring/30 disabled:cursor-not-allowed disabled:opacity-50",
        className,
      )}
      {...props}
    />
  );
}

export function Label({ className, ...props }: LabelHTMLAttributes<HTMLLabelElement>) {
  return <label className={cn("text-xs font-semibold uppercase tracking-wide text-muted", className)} {...props} />;
}

export function Badge({
  className,
  tone = "default",
  ...props
}: HTMLAttributes<HTMLSpanElement> & { tone?: Tone }) {
  const toneClass = {
    default: "border-border bg-surface-soft text-muted",
    danger: "border-[color-mix(in_srgb,var(--danger)_35%,transparent)] bg-[var(--danger-soft)] text-[var(--danger)]",
    warning: "border-[color-mix(in_srgb,var(--warning)_35%,transparent)] bg-[var(--warning-soft)] text-[var(--warning)]",
    info: "border-[color-mix(in_srgb,var(--info)_35%,transparent)] bg-[var(--info-soft)] text-[var(--info)]",
    success: "border-[color-mix(in_srgb,var(--success)_35%,transparent)] bg-[var(--success-soft)] text-[var(--success)]",
  }[tone];

  return <span className={cn("inline-flex items-center rounded-md border px-2 py-0.5 text-xs font-medium", toneClass, className)} {...props} />;
}

export function Separator({ className, ...props }: HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("h-px w-full bg-border", className)} {...props} />;
}

export function Tooltip({ label, children }: { label: string; children: ReactElement }) {
  return (
    <span className="inline-flex" title={label}>
      {children}
    </span>
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
      className={cn("rounded-md border px-3 py-2 text-sm shadow-sm", toneClass, className)}
      {...props}
    />
  );
}
