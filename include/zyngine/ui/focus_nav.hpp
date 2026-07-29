#pragma once

namespace zyngine::ui {

/// Immediate-mode focus ring for encoder / D-pad style UI traversal
/// (3D-printer menu style). Clay has pointer hover but no tab-focus; this fills that gap.
///
/// Wired automatically by ClayHost::beginFrame / endFrame and drawButton().
class FocusNav {
public:
    static FocusNav& instance() {
        static FocusNav n;
        return n;
    }

    /// Apply encoder rotation against last frame's item count; arm activate on click.
    void beginFrame(int encoderSteps, bool encoderClick);

    /// Finalize item count and clamp focus index.
    void endFrame();

    /// Claim the next focus slot (declaration order). Returns the slot index.
    int claimSlot();

    bool isFocused(int slot) const { return slot == focusIndex_; }

    /// Encoder click pending for the focused item. Clears the latch.
    bool takeActivate();

    int focusIndex() const { return focusIndex_; }
    int itemCount() const { return prevCount_; }
    void setFocusIndex(int index) { focusIndex_ = index; }

private:
    FocusNav() = default;

    int focusIndex_ = 0;
    int registerIndex_ = 0;
    int prevCount_ = 0;
    bool activatePending_ = false;
};

}  // namespace zyngine::ui
