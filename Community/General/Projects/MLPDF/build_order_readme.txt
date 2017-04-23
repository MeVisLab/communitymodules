The MLPDF library/modules require the libhpdf library, which is locatd in [Community Modules]/ThirdParty/Sources/libhpdf.
It is also highly recommended to build the MLU3D library/modules, which is located in [Community Modules]/General/Projects/MLU3D/Sources. MLU3D requires MLBaseListExtensions, which is located in [Community Modules]/General/Sources/ML/MLBaseListExtensions.

Therefore, the overall build order should be:

1. [Community Modules]/ThirdParty/Sources/libhpdf
2. [Community Modules]/General/Sources/ML/MLBaseListExtensions
3. [Community Modules]/General/Projects/MLU3D/Sources
4. [Community Modules]/General/Projects/MLPDF/Sources