#pragma once

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

    class Triple;

    class DCVMELFMCAsmInfo : public MCAsmInfoELF {
        void anchor() override;

    public:
        explicit DCVMELFMCAsmInfo(const Triple& TheTriple);
    };

} // namespace llvm
