#include <cstring>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "maptel.h"

using std::pair;
using std::string;
using std::unordered_map;
using std::find;
using std::begin;
using std::end;
using std::cerr;

#ifdef NDEBUG
static const bool debug = false;
#else
static const bool debug = true;
#endif

template <typename T>
void debug_info(T const &info) {
    if (debug) {
        std::ios_base::Init();
        cerr << info;
    }
}

template <typename T, typename... U>
void debug_info(T const &arg, U const &... args) {
    if (debug) {
        std::ios_base::Init();
        cerr << arg;
        debug_info(args...);
    }
}

// Typ okreslajacy id slownika z mapowaniem numerow
using map_id_t = unsigned long;
// Typ przechowujacy mapowania numerow
using tel_map_t = unordered_map<string, string>;
// Typ przechowujacy wszystkie slowniki
using maptels_t = unordered_map<map_id_t, tel_map_t>;
void p(tel_map_t & map) {
for(auto e : map)
	cerr << e.first << ":" << e.second << std::endl;
}	

namespace {
// ostatnio użyty numer slownika - inicjalizowany na -1, tj. maksymalna
// dopuszczalna wartosc zmiennej typu unsigned long (zwiekszenie o 1 spowoduje
// nadanie pierwszemu slownikowi id = 0)
map_id_t counter = -1;

// Zwraca singleton z mapa przechowującą słowniki
maptels_t &maptels_get() {
    static maptels_t *mpt = new maptels_t();
    return *mpt;
}
}

// Sprawdza, czy slownik o zadanym id istnieje
bool maptel_exists(map_id_t id) {
    return maptels_get().find(id) != maptels_get().end();
}

// Tworzy słownik i zwraca liczbę naturalną będącą jego identyfikatorem
unsigned long maptel_create() {
    tel_map_t telmap;
    counter++;
    maptels_get().insert({counter, telmap});

    debug_info("maptel: maptel_create()\n");
    debug_info("maptel: maptel_create: new map id = ", counter, "\n");

    return counter;
}

// Usuwa słownik o identyfikatorze id
void maptel_delete(unsigned long id) {
    if (counter >= id) maptels_get().erase(id);
    debug_info("maptel: maptel_delete(", id, ")\n");
    debug_info("maptel: maptel_delete: map ", id, " deleted\n");
}

// Wstawia do słownika o identyfikatorze id informację o zmianie numeru
// tel_src na numer tel_dst. Nadpisuje ewentualną istniejącą informację
void maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst) {
    debug_info("maptel: maptel_insert(", id, ", ", tel_src, ", ", tel_dst,
               ")\n");

    if (maptel_exists(id)) {

        assert(strlen(tel_src) <= TEL_NUM_MAX_LEN);
        assert(strlen(tel_dst) <= TEL_NUM_MAX_LEN);
        string src(tel_src);
        string dst(tel_dst);
        //bool i = maptels_get()[id].insert({src, dst});
 maptels_get()[id][src]=dst;
	cerr << "src:" << tel_src << ", dst:"<<tel_dst<<std::endl;

        debug_info("maptel: maptel_insert: inserted\n");

    } else {
        debug_info(
            "maptel: maptel_insert: maptel with given id does not "
            "exist; element not inserted ()\n");
    }
}

// Jeśli w słowniku o identyfikatorze id jest informacja o zmianie numeru
// tel_src, to ją usuwa. W przeciwnym przypadku nic nie robi
void maptel_erase(unsigned long id, char const *tel_src) {
    bool is_erased = false;
    if (maptel_exists(id)) {
        assert(strlen(tel_src) < TEL_NUM_MAX_LEN);
        string copy_str(tel_src);
        is_erased = maptels_get()[id].erase(copy_str);
    }
    debug_info("maptel: maptel_erase(", id, ", ", tel_src, ")\n");
    debug_info(is_erased ? "maptel: maptel_erase: erased\n"
                         : "maptel: maptel_erase: nothing to erase\n");
}

// Sprawdza, czy w słowniku o identyfikatorze id jest zapisana zmiana numeru
// tel_src. Podąża ciągiem kolejnych zmian. Zapisuje zmieniony numer w tel_dst.
// Jeśli nie ma zmiany numeru lub zmiany tworzą cykl, to zapisuje w tel_dst
// numer tel_src. Wartość len to wielkość przydzielonej pamięci wskazywanej
// przez tel_dst
void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst,
                      size_t len) {
    assert(strlen(tel_src) < TEL_NUM_MAX_LEN);
    assert(strlen(tel_dst) < TEL_NUM_MAX_LEN);

    debug_info("maptel: maptel_transform(", id, ", ", tel_src, ", ", &tel_dst,
               ", ", len, ")\n");
p(maptels_get()[id]);

    string start_string(tel_src);
    string faster(tel_src);
    if (maptel_exists(id)) {
        auto &map_at_id = maptels_get()[id];
        while (map_at_id.find(start_string) != map_at_id.end()) {
           //cerr << start_string << " " << tel_src << "\n";
            start_string = map_at_id[start_string];
	    if(map_at_id.find(map_at_id[faster]) != map_at_id.end())
	    	faster = map_at_id[map_at_id[faster]];
	    if (start_string == faster) {
		    cerr << "bla\n";
		    // Wykrycie cyklu
                debug_info("maptel: maptel_transform: cycle detected\n");
    //char const *dst = tel_src.c_str();
		cerr << tel_dst << " " << tel_src << std::endl;
    strcpy(tel_dst,tel_src );
		cerr << tel_dst << " " << tel_src << std::endl;
               
		//TODO:Assert + debug
return;
//	       	break;
            }
        }
    } else {
        debug_info(
            "maptel: maptel_transform: maptel with given id does not "
            "exist\n");
    }

    assert(len > start_string.size());
    char const *dst = start_string.c_str();
    strcpy(tel_dst, dst);
    debug_info("maptel: maptel_transform: ", tel_src, " -> ", tel_dst, ", \n");
}

