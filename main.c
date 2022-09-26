#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
using namespace std;
#define FastIO ios::sync_with_stdio(false); std::cin.tie(nullptr); std::cout.tie(nullptr);

#define PrefetchLine 5
//#define indiceNum (46792035)
#define indiceNum (100*1000)
//#define bufferSize 637876 //  for the whole dataset
#define bufferSize 4423 //  for the first 100000 lines
//#define bufferSize 43305 //  for the first 100000 lines

void handleCache(unordered_map<long,long> &cacheBuffer,long &cacheHit, long &cacheMiss,stringstream &cacheS,long groundItem,long timeStep){
    string iter;
    bool flag=false;
    int cacheItem;
    cacheS>>iter;
    cacheItem=stoi(iter);
    // a
    if(cacheBuffer.count(groundItem)){
            flag=true;
            cacheHit++;
            cacheBuffer[groundItem] = timeStep;
    }
    // b
    if(cacheItem){
        // a->b, means b.i has been executed
        // b.ii
        if(flag==false){
            cacheMiss++;
            if(cacheBuffer.size()<=bufferSize) {
                cacheBuffer[groundItem] = timeStep;
            }
            else{
                long minTime=indiceNum;
                long minIndex=0;
                for(auto it:cacheBuffer){
                    if(it.second<minTime){
                        minTime=it.second;
                        minIndex=it.first;
                    }
                }
                cacheBuffer.erase(minIndex);
                cacheBuffer[groundItem]=timeStep;
            }
        }
    } else{
        cacheMiss++;
    }
}

void handlePrefetch(unordered_map<long,long> &prefetchBuffer,long &prefetchHit, long &prefetchMiss,stringstream &prefetchS,long groundItem,long timeStep){
    string iter;
    long prefetchItem;
    prefetchS>>iter;
    if(prefetchBuffer.count(groundItem)){
        prefetchHit++;
    }else{
        prefetchMiss++;
    }
    for(int i=0;i<PrefetchLine;i++){
        prefetchS>>iter;
        prefetchItem=stol(iter);
        if(prefetchItem!=0){
    //        if (prefetchBuffer.find(prefetchItem) != prefetchBuffer.end()) {
            // c.i
            if(prefetchBuffer.count(prefetchItem)){
                prefetchBuffer[prefetchItem] = timeStep;
            } else {
                // c.ii
                if(prefetchBuffer.size()<=bufferSize) {
                    prefetchBuffer[prefetchItem] = timeStep;
                }
                else{
                    long minTime=indiceNum;
                    long minIndex=0;
                    for(auto it:prefetchBuffer){
                        if(it.second<minTime){
                            minTime=it.second;
                            minIndex=it.first;
                        }
                    }
                    prefetchBuffer.erase(minIndex);
                    prefetchBuffer[prefetchItem]=timeStep;
                }
            }
        }
        prefetchS>>iter;
    }
}
// fin1.open(indiceFile,ios::in);
// fin2.open(cacheFile,ios::in);
// fin3.open(prefetchFile,ios::in);
void run (fstream &fin1, fstream &fin2,fstream &fin3){
    string indicesLine,cacheLine,prefetchLine;
    long timeStep=0,cacheHit=0,cacheMiss=0,prefetchHit=0,prefetchMiss=0;
    unordered_map<long,long> cacheBuffer;
    // unordered_map<long,long> prefetchBuffer;
       for(int i =0;i<100000;i++){
        getline(fin1, indicesLine);
        getline(fin2, cacheLine);
        getline(fin3, prefetchLine);

        //cout<<"timeStep: "<<timeStep<<endl;
        stringstream indicesS(indicesLine);
        stringstream cacheS(cacheLine);
        stringstream prefetchS(prefetchLine);
        string iter;
        vector<long> prefetchVec;
        //get ground truth number
        long groundItem;
        while(indicesS>>iter){
            groundItem=stol(iter);
        }
        //handle cache
        //cout<<groundItem<<endl;
        handleCache(cacheBuffer,cacheHit,cacheMiss,cacheS,groundItem,timeStep);
        //handle prefetch
        handlePrefetch(cacheBuffer,prefetchHit,prefetchMiss,prefetchS,groundItem,timeStep);
        timeStep++;
       }


    cout<<"cacheHit: "<<cacheHit<<endl;
    // cout<<"cacheMiss: "<<cacheMiss<<endl;
    // cout<<"prefetchHit: "<<prefetchHit<<endl;
    // cout<<"prefetchMiss: "<<prefetchMiss<<endl;


}

int main(){
    FastIO
    fstream fin1,fin2,fin3;
    string indiceFile="../datasets/indices.txt";
    string cacheFile="../datasets/fbgemm_t856_bs65536_15_cache_opt_trace.txt";
    string prefetchFile="../datasets/fbgemm_t856_bs65536_15_prefetch_trace.txt";

    fin1.open(indiceFile,ios::in);
    fin2.open(cacheFile,ios::in);
    fin3.open(prefetchFile,ios::in);

    struct timespec start, stop;
    double time;
    if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
    run(fin1,fin2,fin3);
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}
    time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

    std::cout << "time:"<<time <<"s"<< std::endl;
    return 0;
}
