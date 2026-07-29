#include "zyngine/ui/focus_nav.hpp"

namespace zyngine::ui {

void FocusNav::beginFrame(int encoderSteps, bool encoderClick) {
    if (prevCount_ > 0 && encoderSteps != 0) {
        int idx = focusIndex_ + encoderSteps;
        idx %= prevCount_;
        if (idx < 0) {
            idx += prevCount_;
        }
        focusIndex_ = idx;
    }
    activatePending_ = encoderClick;
    registerIndex_ = 0;
}

void FocusNav::endFrame() {
    prevCount_ = registerIndex_;
    if (prevCount_ <= 0) {
        focusIndex_ = 0;
        return;
    }
    if (focusIndex_ >= prevCount_) {
        focusIndex_ = prevCount_ - 1;
    }
}

int FocusNav::claimSlot() { return registerIndex_++; }

bool FocusNav::takeActivate() {
    if (!activatePending_) {
        return false;
    }
    activatePending_ = false;
    return true;
}

}  // namespace zyngine::ui
