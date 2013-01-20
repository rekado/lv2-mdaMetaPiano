f = File.open("samples.raw", "r")

kgrp = []

kgrp[ 0]={};  kgrp[ 0][:pos] = 0;       kgrp[ 0][:end] = 36275; 
kgrp[ 1]={};  kgrp[ 1][:pos] = 36278;   kgrp[ 1][:end] = 83135; 
kgrp[ 2]={};  kgrp[ 2][:pos] = 83137;   kgrp[ 2][:end] = 146756;
kgrp[ 3]={};  kgrp[ 3][:pos] = 146758;  kgrp[ 3][:end] = 204997;
kgrp[ 4]={};  kgrp[ 4][:pos] = 204999;  kgrp[ 4][:end] = 244908;
kgrp[ 5]={};  kgrp[ 5][:pos] = 244910;  kgrp[ 5][:end] = 290978;
kgrp[ 6]={};  kgrp[ 6][:pos] = 290980;  kgrp[ 6][:end] = 342948;
kgrp[ 7]={};  kgrp[ 7][:pos] = 342950;  kgrp[ 7][:end] = 391750;
kgrp[ 8]={};  kgrp[ 8][:pos] = 391752;  kgrp[ 8][:end] = 436915;
kgrp[ 9]={};  kgrp[ 9][:pos] = 436917;  kgrp[ 9][:end] = 468807;
kgrp[10]={};  kgrp[10][:pos] = 468809;  kgrp[10][:end] = 492772;
kgrp[11]={};  kgrp[11][:pos] = 492774;  kgrp[11][:end] = 532293;
kgrp[12]={};  kgrp[12][:pos] = 532295;  kgrp[12][:end] = 560192;
kgrp[13]={};  kgrp[13][:pos] = 560194;  kgrp[13][:end] = 574121;
kgrp[14]={};  kgrp[14][:pos] = 574123;  kgrp[14][:end] = 586343;


(0..14).each do |i|
  f.seek(2 * kgrp[i][:pos])
  kgrp[i][:data] = f.read(2 * (kgrp[i][:end] - kgrp[i][:pos]))
end
f.close

kgrp[ 0][:name] = "0c"
kgrp[ 1][:name] = "0e"
kgrp[ 2][:name] = "0g"
kgrp[ 3][:name] = "1c"
kgrp[ 4][:name] = "1e"
kgrp[ 5][:name] = "1g"
kgrp[ 6][:name] = "2c"
kgrp[ 7][:name] = "2e"
kgrp[ 8][:name] = "2g"
kgrp[ 9][:name] = "3c"
kgrp[10][:name] = "3e"
kgrp[11][:name] = "3g"
kgrp[12][:name] = "4c"
kgrp[13][:name] = "4e"
kgrp[14][:name] = "4a"

kgrp.each do |k|
  f = File.open("#{k[:name]}.raw", "w")
  f.write k[:data]
  f.close
end
