# include "ReadingSync.h"
// Determine if its time to take a reading 
// Determine if its time to send a reading

// --------------------------------------------------------------------- TIMING
int ReadingSync::getStartOfDayUnixTime(int currentTime) {
    int daysSinceEpoch=currentTime / (SECS_IN_DAY);
    return (daysSinceEpoch * SECS_IN_DAY);
}

int ReadingSync::getSecsSinceStartOfDay(int currentTime) {
    return (currentTime - getStartOfDayUnixTime(currentTime));
}

int ReadingSync::getRemainingSecsUntilSample(int currentTime) {
    int remainingSecs = 0;
    int secsSinceStartOfDay = getSecsSinceStartOfDay(currentTime);
    int secsSinceLastReading = (secsSinceStartOfDay % secs_between_readings);
    if(secsSinceLastReading!=0)
    {
        remainingSecs = secs_between_readings - secsSinceLastReading;
    }
//std::cout << "sssod=" << secsSinceStartOfDay << "||rs=" << remainingSecs << "||sbr=" << secs_between_readings << "||sslr=" << secsSinceLastReading << "\n";   
    return remainingSecs;   
}

bool ReadingSync::isTimeToSample(int currentTime) {
    int secsSinceStartOfDay = getSecsSinceStartOfDay(currentTime);  
    int remainingSecs = getRemainingSecsUntilSample(currentTime);
    bool timeToSample = (remainingSecs == 0) && (secsSinceStartOfDay!=last_read_secs);
//std::cout << "tts=" << timeToSample << "||sssod=" << secsSinceStartOfDay << "|rs=" << remainingSecs << "\n\n";        
    if(timeToSample) {
        last_read_secs = secsSinceStartOfDay;   
    }
    return timeToSample;
}

bool ReadingSync::isTimeToPreHeat(int currentTime) {
//std::cout << "grsus.ct=" << getRemainingSecsUntilSample(currentTime) << "\n\n";
    int remainingSecs = getRemainingSecsUntilSample(currentTime);
//std::cout << "grsus.ph=" << remainingSecs << "||ct=" << currentTime << "||phs=" << pre_heat_secs << "|rs=" << remainingSecs << "\n\n";    
    bool timeToPreHeat=false;
    if((remainingSecs >0) && (remainingSecs<=pre_heat_secs))
      timeToPreHeat=true;
    return timeToPreHeat;
}

bool ReadingSync::isTimeToSendReading(int currentTime) {
    return getSecsSinceStartOfDay(currentTime) >= next_send_secs;
}

// --------------------------------------------------------------------- CALIBRATION
void ReadingSync::startCalibrating(int currentTime) {
    calibration_start_time=currentTime;
    _stage=PRE_HEAT_CALIBRATING;
}

void ReadingSync::setCalibratingComplete() {
    _stage=CONTINUE;
}

// --------------------------------------------------------------------- USER SAMPLING
void ReadingSync::startUserSampling(int currentTime) {
    user_sampling_start_time=currentTime;
    _stage=PRE_HEAT_USER_SAMPLING;
}

// --------------------------------------------------------------------- SCHEDULED SAMPLING
void ReadingSync::setSamplingComplete() {
    if(_stage==SAMPLING) next_send_secs = last_read_secs + (rand() % (secs_between_readings-120));   
    _stage=CONTINUE;
}

void ReadingSync::setReadingSent() {
    next_send_secs = C_MAX_INT;
    _stage=CONTINUE;
}

// --------------------------------------------------------------------- STAGE
ReadingSync::Stage ReadingSync::getStage(int currentTime) {
    if(_stage==PRE_HEAT_USER_SAMPLING) {
        if(currentTime>=(user_sampling_start_time+pre_heat_secs)) {
            _stage=USER_SAMPLING;
        }
        return _stage;
    } else if(_stage==USER_SAMPLING) {
        return _stage;
    } else if(_stage==PRE_HEAT_CALIBRATING) {
        if(currentTime>=(calibration_start_time+pre_heat_secs)) {
            _stage=CALIBRATING;
        }
        return _stage;
    } else if(_stage==CALIBRATING) {
        return _stage;
    } else if(isTimeToSample(currentTime) || _stage==SAMPLING) {
        _stage=SAMPLING;
        return _stage;  
    } else if(isTimeToPreHeat(currentTime)) {
        _stage=PRE_HEATING;
        return _stage;
    } else if(isTimeToSendReading(currentTime)) {
        _stage=SEND_READING;
        return _stage;
    }
    return CONTINUE;
}
