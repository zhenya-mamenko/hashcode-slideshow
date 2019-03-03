//
//  main.cpp
//  SlideShow
//
//  Created by Hackathon Event on 28/02/2019.
//  Copyright © 2019 Hackathon Event. All rights reserved.
//

#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

struct Tag {
    explicit Tag(const long long val) : value(val) {}
    long long value;
    bool operator<(const Tag& other) const {
        return value < other.value;
    }
};

enum Orientation {
    Orientation_H = 0,
    Orientation_V = 1
};

struct Photo {
    int identifier = -1;
    Orientation orientation = Orientation_H;
    std::set<Tag> tags;
    bool used = false;

    void dump() const {
        printf("%d %c %d", (int)identifier, orientation == Orientation_H ? 'H' : 'V', (int)tags.size());
        
        for (auto tag : tags) {
            printf(" %llu", tag.value);
        }
        
        printf("\n");
    }
};

struct Slide {
    int ids[2] = {-1, -1};
    bool used = false;
    std::set<Tag> tags;
};

struct SlideShow {
    std::vector<Slide> slides;
    
    void dumpToFile(const char *fname) const {
        std::ofstream file(fname);
        file << slides.size() << std::endl;
        
        for (auto & slide : slides) {
            file << slide.ids[0];
            
            if (slide.ids[1] == -1) {
                file << "\n";
            }
            else {
                file << " " << slide.ids[1] << "\n";
            }
        }
    }
    
    void addSlide(const Slide& s) {
        slides.push_back(s);
    }
};

std::vector<Photo> readInput(const char *fname);
Photo parsePhotoString(int id, std::string& line);
void sortPhotos(std::vector<Photo>& photos);
std::vector<Slide> convertPhotosToSlides(const std::vector<Photo>& phs);
int findBestMatch(const std::vector<Slide>& phs, int pid, bool findSameTagsCount);

int main(int argc, const char * argv[]) {
    auto photos2 = readInput("e_shiny_selfies.txt");
    sortPhotos(photos2);
    auto slides = convertPhotosToSlides(photos2);
    
    int proc_cnt = 0;
    int i = 0;
    SlideShow ss;
    // photos[i].dump();
    ss.addSlide(slides[i]);
    
    while (proc_cnt + 1 < slides.size())
    {
        if (slides[i].used) {
            break;
        }
        
        int j = findBestMatch(slides, i, true);
        
        if (j == -1) {
            j = findBestMatch(slides, i, false);
        }
        
        // photos[j].dump();
        ss.addSlide(slides[j]);
        slides[i].used = true;
        i = j;
        proc_cnt++;
        
        if (proc_cnt % 100 == 0) {
            std::cout << proc_cnt << std::endl;
        }
        

    }
    
    ss.dumpToFile("outE.txt");
    return 0;
}

std::vector<Photo> readInput(const char *fname) {
    std::vector<Photo> res;
    std::ifstream file(fname);
    std::string line;
    
    if (getline(file, line)) {
        int numPhotos = atoi(line.c_str());
        
        for (int i = 0; i < numPhotos; ++i) {
            getline(file, line);
            res.push_back(parsePhotoString(i, line));
        }
    }
    
    return res;
}

long long convertTag(char *s)
{
    int l = strlen(s);
    long long result = 0;
    for (int j = 0; j < l; ++j) {
        result = result * 36 + ((s[j] >= '0' && s[j] <= '9') ? (s[j] - '0') : (s[j] - 'a' + 10));
    }
    return (result);
}

Photo parsePhotoString(int id, std::string& line) {
    Photo photo;
    photo.identifier = id;
    char *pch = strtok(&line[0], " ");
    photo.orientation = pch[0] == 'H' ? Orientation_H : Orientation_V;
    pch = strtok(NULL, " ");
    int numTags = atoi(pch);
    
    for (int j = 0; j < numTags; ++j) {
        pch = strtok(NULL, " ");
        photo.tags.insert(Tag(convertTag(pch)));
    }
    
    return photo;
}

void sortPhotos(std::vector<Photo>& photos) {
    std::sort(photos.begin(), photos.end(), [](auto p1, auto p2) {
        if (p1.orientation != p2.orientation) {
            return p1.orientation < p2.orientation;
        }
        return p1.tags.size() > p2.tags.size();
    });
}

int findBestMatch(const std::vector<Slide>& phs, int pid, bool findSameTagsCount) {
    int bestDiff = -1;
    int bestId = -1;
    for (int i = phs.size() - 1; i >= 0; --i) {
        if (phs[i].used || i == pid) continue;
        
        if (findSameTagsCount && phs[i].tags.size() < phs[pid].tags.size()) {
            continue;
        }
        
        if (findSameTagsCount && phs[i].tags.size() > phs[pid].tags.size()) {
            break;
        }
        
        int res = count_if(phs[pid].tags.begin(), phs[pid].tags.end(), [&](auto tag) {return phs[i].tags.find(tag) != phs[i].tags.end();});
        
        int minTagCount = (int)std::min(phs[pid].tags.size(), phs[i].tags.size());
        int diff = abs(minTagCount / 2 - res);
        
        if (diff == 0) {
            return i;
        }
        
        if (bestDiff == -1 || diff < bestDiff) {
            bestDiff = diff;
            bestId = i;
        }
    }
    
    return bestId;
}

std::vector<Slide> convertPhotosToSlides(const std::vector<Photo>& phs) {
    std::vector<Slide> res;
    size_t j = phs.size();
    for (size_t i = 0; i < phs.size(); ++i) {
        if (phs[i].orientation == Orientation_H) {
            Slide s;
            s.ids[0] = phs[i].identifier;
            s.tags = phs[i].tags;
            res.push_back(s);
        }
        else {
            if (i + 1 < j) {
                Slide s;
                s.ids[0] = phs[i].identifier;
                s.ids[1] = phs[j - 1].identifier;
                s.tags = phs[i].tags;
                
                for (auto t : phs[j - 1].tags) {
                    s.tags.insert(t);
                }
                
                --j;
                res.push_back(s);
            }
        }
    }
    return res;
}
