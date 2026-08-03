pragma Singleton
import QtQuick

QtObject {
    id: root

    // Emerald Odyssey Palette - Core Brand Colors
    readonly property color primary: "#059669"          // Deep Emerald
    readonly property color primaryHover: "#10B981"     // Medium Emerald
    readonly property color primaryActive: "#047857"    // Dark Emerald
    readonly property color primarySubtle: "#022C22"    // Ultra Dark Emerald (Accents/Highlights)

    readonly property color secondary: "#14B8A6"        // Teal Accent
    readonly property color secondaryHover: "#2DD4BF"   // Light Teal
    readonly property color secondaryActive: "#0D9488"  // Dark Teal

    // Backgrounds & Surface Hierarchy
    readonly property color bgBase: "#090D16"           // Odyssey Base Dark Background
    readonly property color bgSurface: "#111827"        // Card / Panel Surface
    readonly property color bgSurfaceElevated: "#1F2937"// Popover / Modal / Dropdown Surface
    readonly property color bgSurfaceHover: "#374151"   // Interactive Surface Hover State

    // Borders & Dividers
    readonly property color borderSubtle: "#1F2937"
    readonly property color borderMedium: "#374151"
    readonly property color borderBright: "#059669"

    // Typography & Content Colors
    readonly property color textPrimary: "#F9FAFB"
    readonly property color textSecondary: "#9CA3AF"
    readonly property color textMuted: "#6B7280"
    readonly property color textOnPrimary: "#FFFFFF"

    // Status & System Colors
    readonly property color success: "#10B981"
    readonly property color warning: "#F59E0B"
    readonly property color error: "#EF4444"
    readonly property color info: "#3B82F6"

    readonly property color successBg: "#064E3B"
    readonly property color warningBg: "#78350F"
    readonly property color errorBg: "#7F1D1D"
    readonly property color infoBg: "#1E3A8A"

    // Overlay & Utility
    readonly property color overlayModal: "#CC000000"
    readonly property color transparent: "transparent"
}