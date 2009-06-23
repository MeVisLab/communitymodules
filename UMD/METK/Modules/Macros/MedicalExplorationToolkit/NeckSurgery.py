COL_LYMPH     = "1 1 0.59"
COL_MUSCLE    = "0.39 0.16 0.08"
COL_VEIN      = "0.31 0.31 0.98"
COL_ARTERY    = "0.94 0.20 0.20"
COL_BONE      = "1 1 1"
COL_NERVE     = "0.94 0.73 0.31"
COL_TRACHEA   = "1 0.90 0.71"
COL_PHARYNX   = "1 0.74 0.59"
COL_ESOPHAGUS = "1 0.74 0.59"
COL_LUNG      = "1 0.90 0.71"
COL_GLAND     = "0.71 0.59 0.43"
COL_LARYNX    = "0.66 0.82 1"
COL_CARTILAGO = "0.66 0.82 1"
COL_TUMOR     = "1 1 0.78"

COL_EMPHASIZE = "1 0 0"
TRA_EMPHASIZE = "0.0"
IMP_EMPHASIZE = "1.5"

TRA_LYMPH     = "0.0"
TRA_MUSCLE    = "0.5"
TRA_VEIN      = "0.4"
TRA_ARTERY    = "0.4"
TRA_BONE      = "0.8"
TRA_NERVE     = "0.0"
TRA_TRACHEA   = "0.0"
TRA_PHARYNX   = "0.8"
TRA_ESOPHAGUS = "0.8"
TRA_LUNG      = "0.0"
TRA_GLAND     = "0.3"
TRA_LARYNX    = "0.4"
TRA_CARTILAGO = "0.4"
TRA_TUMOR     = "0.0"

IMP_LYMPH     = "1.1"
IMP_MUSCLE    = "0.5"
IMP_VEIN      = "0.6"
IMP_ARTERY    = "0.6"
IMP_BONE      = "0.2"
IMP_NERVE     = "1.0"
IMP_TRACHEA   = "0.5"
IMP_PHARYNX   = "0.2"
IMP_ESOPHAGUS = "0.2"
IMP_LUNG      = "0.2"
IMP_GLAND     = "0.3"
IMP_LARYNX    = "0.6"
IMP_CARTILAGO = "0.6"
IMP_TUMOR     = "2.0"

LYMPHNODE = "Lymphnode"         # Structure and StructureGroup
TUMOR     = "Tumor"             # Structure and StructureGroup
BONE      = "Bone"              # Structure and StructureGroup
MUSCLE    = "Muscle"            # Structure and StructureGroup
VESSEL    = "Vessel"            # StructureGroup for Artery and Vein
VEIN      = "Vein"
ARTERY    = "Artery"
NERVE     = "Nerve"             # Structure and StructureGroup
GLAND     = "Gland"             # StructureGroup for Salivary Gland and Thyroidea
SALGLAND  = "Salivary Gland"
THYGLAND  = "Thyroidea"         # Schilddruese
LARYNX    = "Larynx"            # Structure and StructureGroup for Cartilago
CARTILAGO = "Cartilago"
RESPTRACT = "Respiratory Tract" # StructureGroup for Trachea, Pharynx, Lung
TRACHEA   = "Trachea"
PHARYNX   = "Pharynx"
ESOPHAGUS = "Esophagus"
LUNG      = "Lung"
USERDEF   = "UserDefined"       # Structure and StructureGroup


# for LAY_DESCRIPTION
INF_LEVEL               = "Level" # Lymph node level
INF_TYPE                = "Type"  # Kind of tumor
INF_TCLASS              = "T_Classification"  # TNM classification
INF_NCLASS              = "N_Classification"  # TNM classification
INF_STADIUM             = "Stadium"           # TNM classification


def getStandardColor(structure):
   if   structure == LYMPHNODE:
      color = COL_LYMPH
   elif structure == MUSCLE:
      color = COL_MUSCLE
   elif structure == VEIN:
      color = COL_VEIN
   elif structure == ARTERY:
      color = COL_ARTERY
   elif structure == BONE:
      color = COL_BONE
   elif structure == NERVE:
      color = COL_NERVE
   elif structure == TRACHEA:
      color = COL_TRACHEA
   elif structure == PHARYNX:
      color = COL_PHARYNX
   elif structure == LUNG:
      color = COL_LUNG
   elif structure == SALGLAND:
      color = COL_GLAND
   elif structure == THYGLAND:
      color = COL_GLAND
   elif structure == LARYNX:
      color = COL_LARYNX
   elif structure == CARTILAGO:
      color = COL_CARTILAGO
   elif structure == TUMOR:
      color = COL_TUMOR
   else:
      color = "0.8 0.8 0.8"
   return color


def getStandardTransparency(structure):
   if   structure == LYMPHNODE:
      transparency = TRA_LYMPH
   elif structure == MUSCLE:
      transparency = TRA_MUSCLE
   elif structure == VEIN:
      transparency = TRA_VEIN
   elif structure == ARTERY:
      transparency = TRA_ARTERY
   elif structure == BONE:
      transparency = TRA_BONE
   elif structure == NERVE:
      transparency = TRA_NERVE
   elif structure == TRACHEA:
      transparency = TRA_TRACHEA
   elif structure == PHARYNX:
      transparency = TRA_PHARYNX
   elif structure == LUNG:
      transparency = TRA_LUNG
   elif structure == SALGLAND:
      transparency = TRA_GLAND
   elif structure == THYGLAND:
      transparency = TRA_GLAND
   elif structure == LARYNX:
      transparency = TRA_LARYNX
   elif structure == CARTILAGO:
      transparency = TRA_CARTILAGO
   elif structure == TUMOR:
      transparency = TRA_TUMOR
   elif structure == USERDEF:
      transparency = "0.0"
   else:
      transparency = "0.5"
   return transparency


def getStandardImportance(structure):
   if   structure == LYMPHNODE:
      importance = IMP_LYMPH
   elif structure == MUSCLE:
      importance = IMP_MUSCLE
   elif structure == VEIN:
      importance = IMP_VEIN
   elif structure == ARTERY:
      importance = IMP_ARTERY
   elif structure == BONE:
      importance = IMP_BONE
   elif structure == NERVE:
      importance = IMP_NERVE
   elif structure == TRACHEA:
      importance = IMP_TRACHEA
   elif structure == PHARYNX:
      importance = IMP_PHARYNX
   elif structure == LUNG:
      importance = IMP_LUNG
   elif structure == SALGLAND:
      importance = IMP_GLAND
   elif structure == THYGLAND:
      importance = IMP_GLAND
   elif structure == LARYNX:
      importance = IMP_LARYNX
   elif structure == CARTILAGO:
      importance = IMP_CARTILAGO
   elif structure == TUMOR:
      importance = IMP_TUMOR
   elif structure == USERDEF:
      importance = "0.8"
   else:
      importance = "0.5"
   return importance
