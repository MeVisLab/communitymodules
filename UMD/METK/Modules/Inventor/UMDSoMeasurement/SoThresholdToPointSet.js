

function init(){
  //int color
  ctx.field("colorFlag").setBoolValue(false);
  ctx.field("setColor").setBoolValue(true);
}

function listenColorFlag(){
  var flag = arguments[0].value;
  ctx.field("setColor").setBoolValue(!flag);
}

//# MeVis signature v1
//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBAKyWeOZdMOT9Sj40ZS32A8i8GAjHkcHh3QT05EJBoHkqE3S0/FOcZM6SFkMLVcjiDmzBdoc9REZUIl5061137BkCARE=:mbUARszcrJxTM2e3H2YskVHojl15mYg4OBjlYsSeJfjTYEVwkgvJJ0eFJB4r7/u64QtiMUxLsor5/MgAGO4iCA==
//# owner: MeVis Internal Developer
//# date: 2005-08-18T18:57:42
//# hash: aWx4n8Bi6f0wmLnog1w2hG6YSeswHBJ6u/kRYuKgmZcoKWwoFlQgKezH2to/uYoMWS3qrCYoulAqO+aSqaRbmA==
//# MeVis end
