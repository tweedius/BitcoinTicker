#include "Arduino.h"
#include "BitcoinTicker.h"

//{"sequence":598805431,"bids":[["380.11","11.263",4]],"asks":[["380.12","29.91011392",2]]}
//{"open":"365.53000000","high":"385.05000000","low":"365.53000000","volume":"9766.33459012","volume_30day":"250122.75853462"}

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

String add_commas(float _infloat){
  
}

String formatTime(int _hour, int _minute, int _second){

  String _temp_minute, _temp_second, _out_string;

  if(_minute < 10){
    _temp_minute = "0";
    _temp_minute.concat(_minute);
  }
  else{
    _temp_minute = String(_minute);
  }
  
  if(_second < 10){
    _temp_second = "0";
    _temp_second.concat(_second);
  }
  else{
    _temp_second = String(_second);
  }

  _out_string = String(_hour);
  _out_string.concat(":");
  _out_string.concat(_temp_minute);
  _out_string.concat(":");
  _out_string.concat(_temp_second);

  return _out_string;
    
}


