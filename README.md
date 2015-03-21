# MazedTail
cd /Users/lilil/Documents/cocos2d-x-3.2

cocos new MazedTail -p com.LilCompany.MazedTale -l cpp -d ~/LilCompany

cocos run -s ~/LilCompany/MazedTail -p ios

write into /Users/lilil/LilCompany/MazedTail/proj.android/jni/Android.mk new classes to LOCAL_SRC_FILES

MBP-lilil:proj.android lilil$ pwd /Users/lilil/LilCompany/MazedTail/proj.android MBP-lilil:proj.android lilil$ python build_native.py -p 21
