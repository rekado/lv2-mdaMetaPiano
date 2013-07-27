f = File.open("samples.raw", "r")

kgrp = []

kgrp[ 0]={};  kgrp[ 0][:pos] = 0;       kgrp[ 0][:end] = 8476;    
kgrp[ 1]={};  kgrp[ 1][:pos] = 8477;    kgrp[ 1][:end] = 16248;   
kgrp[ 2]={};  kgrp[ 2][:pos] = 16249;   kgrp[ 2][:end] = 34565;   
kgrp[ 3]={};  kgrp[ 3][:pos] = 34566;   kgrp[ 3][:end] = 41384;   
kgrp[ 4]={};  kgrp[ 4][:pos] = 41385;   kgrp[ 4][:end] = 45760;   
kgrp[ 5]={};  kgrp[ 5][:pos] = 45761;   kgrp[ 5][:end] = 65211;   
kgrp[ 6]={};  kgrp[ 6][:pos] = 65212;   kgrp[ 6][:end] = 72897;   
kgrp[ 7]={};  kgrp[ 7][:pos] = 72898;   kgrp[ 7][:end] = 78626;   
kgrp[ 8]={};  kgrp[ 8][:pos] = 78627;   kgrp[ 8][:end] = 100387;  
kgrp[ 9]={};  kgrp[ 9][:pos] = 100388;  kgrp[ 9][:end] = 116297;  
kgrp[10]={};  kgrp[10][:pos] = 116298;  kgrp[10][:end] = 127661; 
kgrp[11]={};  kgrp[11][:pos] = 127662;  kgrp[11][:end] = 144113; 
kgrp[12]={};  kgrp[12][:pos] = 144114;  kgrp[12][:end] = 152863; 
kgrp[13]={};  kgrp[13][:pos] = 152864;  kgrp[13][:end] = 173107; 
kgrp[14]={};  kgrp[14][:pos] = 173108;  kgrp[14][:end] = 192734; 
kgrp[15]={};  kgrp[15][:pos] = 192735;  kgrp[15][:end] = 204598; 
kgrp[16]={};  kgrp[16][:pos] = 204599;  kgrp[16][:end] = 218995; 
kgrp[17]={};  kgrp[17][:pos] = 218996;  kgrp[17][:end] = 233801; 
kgrp[18]={};  kgrp[18][:pos] = 233802;  kgrp[18][:end] = 248011; 
kgrp[19]={};  kgrp[19][:pos] = 248012;  kgrp[19][:end] = 265287; 
kgrp[20]={};  kgrp[20][:pos] = 265288;  kgrp[20][:end] = 282255; 
kgrp[21]={};  kgrp[21][:pos] = 282256;  kgrp[21][:end] = 293776; 
kgrp[22]={};  kgrp[22][:pos] = 293777;  kgrp[22][:end] = 312566; 
kgrp[23]={};  kgrp[23][:pos] = 312567;  kgrp[23][:end] = 330200; 
kgrp[24]={};  kgrp[24][:pos] = 330201;  kgrp[24][:end] = 348889; 
kgrp[25]={};  kgrp[25][:pos] = 348890;  kgrp[25][:end] = 365675; 
kgrp[26]={};  kgrp[26][:pos] = 365676;  kgrp[26][:end] = 383661; 
kgrp[27]={};  kgrp[27][:pos] = 383662;  kgrp[27][:end] = 393372; 
kgrp[28]={};  kgrp[28][:pos] = 383662;  kgrp[28][:end] = 393372; 
kgrp[29]={};  kgrp[29][:pos] = 393373;  kgrp[29][:end] = 406045; 
kgrp[30]={};  kgrp[30][:pos] = 406046;  kgrp[30][:end] = 414486; 
kgrp[31]={};  kgrp[31][:pos] = 406046;  kgrp[31][:end] = 414486; 
kgrp[32]={};  kgrp[32][:pos] = 414487;  kgrp[32][:end] = 422408; 

(0..32).each do |i|
  f.seek(2 * kgrp[i][:pos])
  kgrp[i][:data] = f.read(2 * (kgrp[i][:end] - kgrp[i][:pos]))
end
f.close

kgrp[ 0][:name] = "0c-0"
kgrp[ 1][:name] = "0c-1"
kgrp[ 2][:name] = "0c-2"

kgrp[ 3][:name] = "0g-0"
kgrp[ 4][:name] = "0g-1"
kgrp[ 5][:name] = "0g-2"

kgrp[ 6][:name] = "1c-0"
kgrp[ 7][:name] = "1c-1"
kgrp[ 8][:name] = "1c-2"

kgrp[ 9][:name] = "1g-0"
kgrp[10][:name] = "1g-1"
kgrp[11][:name] = "1g-2"

kgrp[12][:name] = "2c-0"
kgrp[13][:name] = "2c-1"
kgrp[14][:name] = "2c-2"

kgrp[15][:name] = "2g-0"
kgrp[16][:name] = "2g-1"
kgrp[17][:name] = "2g-2"

kgrp[18][:name] = "3c-0"
kgrp[19][:name] = "3c-1"
kgrp[20][:name] = "3c-2"

kgrp[21][:name] = "3g-0"
kgrp[22][:name] = "3g-1"
kgrp[23][:name] = "3g-2"

kgrp[24][:name] = "4c-0"
kgrp[25][:name] = "4c-1"
kgrp[26][:name] = "4c-2"

kgrp[27][:name] = "4g-0"
kgrp[28][:name] = "4g-1"
kgrp[29][:name] = "4g-2"

kgrp[30][:name] = "5c-0"
kgrp[31][:name] = "5c-1"
kgrp[32][:name] = "5c-2"

kgrp.each do |k|
  f = File.open("#{k[:name]}.raw", "w")
  f.write k[:data]
  f.close
end
