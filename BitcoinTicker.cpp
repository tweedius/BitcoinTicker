#include "Arduino.h"
#include "BitcoinTicker.h"

//{"sequence":598805431,"bids":[["380.11","11.263",4]],"asks":[["380.12","29.91011392",2]]}
//{"open":"365.53000000","high":"385.05000000","low":"365.53000000","volume":"9766.33459012","volume_30day":"250122.75853462"}
//{"iso":"2016-02-11T03:19:39.488Z","epoch":1455160779.488}

float get_coinbase_epoch(String _json_data){

  return _json_data.substring(_json_data.indexOf("epoch") + 7, _json_data.indexOf("}")).toFloat();
  
}

float get_coinbase_bid(String _json_data){

  return _json_data.substring(_json_data.indexOf("bids") + 9, _json_data.indexOf("\"",_json_data.indexOf("bids")+9) + 1).toFloat();

}

float get_coinbase_ask(String _json_data){

  return _json_data.substring(_json_data.indexOf("asks") + 9, _json_data.indexOf("\"",_json_data.indexOf("asks")+9) + 1).toFloat();

}


float get_coinbase_open(String _json_data){

  return _json_data.substring(_json_data.indexOf("open") + 7, _json_data.indexOf("\"",_json_data.indexOf("open")+7) + 1).toFloat();

}

float get_coinbase_high(String _json_data){

  return _json_data.substring(_json_data.indexOf("high") + 7, _json_data.indexOf("\"",_json_data.indexOf("high")+7) + 1).toFloat();

}

float get_coinbase_low(String _json_data){

  return _json_data.substring(_json_data.indexOf("low") + 6, _json_data.indexOf("\"",_json_data.indexOf("low")+6) + 1).toFloat();
  
}

float sci_to_float(String _instring){

  float _base, _exponent;
  String _temp;

  _temp = _instring.substring(0, _instring.indexOf("E"));
  //Serial.print("base: ");
  //Serial.println(_temp);
  _base = _temp.toFloat();

  _temp = _instring.substring(_instring.indexOf("E")+1, _instring.length());
  //Serial.print("exponent: ");
  //Serial.println(_temp);
  _exponent = _temp.toFloat();
 //priceMarketCap = 5.687923626 * pow(10, 9);

  return  _base * pow(10, _exponent);


  //return _instring.substring(0, _instring.indexOf("E")).toFloat() * pow(10, _instring.substring(_instring.indexOf("E")+1, _instring.length()).toFloat);

}


String add_commas(String _instring){

  String _outstring;
  int insertPosition = _instring.length();

  //Serial.print("_instring.length()= ");
  //Serial.println(_instring.length());

  if(_instring.length() >=4){
    while(insertPosition > 0){
      
      if(insertPosition-3>0){
        _outstring = _instring.substring(insertPosition - 3, insertPosition) + _outstring;
        _outstring = "," + _outstring;
      }
      else{
        _outstring = _instring.substring(0, insertPosition) + _outstring;
      }
      insertPosition = insertPosition - 3;

    }
  }
      
  return _outstring;
}


String formatTime(int _hour, int _minute){

  String _temp_hour, _temp_minute, _out_string;

  if(_hour>12){
    _temp_hour = String(_hour - 12);
  }
  else{
    _temp_hour = String(_hour);
  }
  
  if(_minute < 10){
    _temp_minute = "0";
    _temp_minute.concat(_minute);
  }
  else{
    _temp_minute = String(_minute);
  }

  _out_string = String(_temp_hour);
  _out_string.concat(":");
  _out_string.concat(_temp_minute);
  if(_hour < 13){
    _out_string.concat(" AM");    
  }
  else{
    _out_string.concat(" PM");    
  }

  return _out_string;
    
}


