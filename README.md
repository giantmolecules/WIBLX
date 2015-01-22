# WIBLX

BLX/WIBLX_01.CPP is the code I've been potting on the BLX. It publishes values for RF and gyro. WEB/index16.html is the page that subscribes to that info. The Page uses three.js to render the 3d cube. Color is determined by measured RF vlaues, orientation by accelerometer data. The page needs a PHP proxy to obscure the access token. Work has been done towards this, but has not been successfully implemented. 
