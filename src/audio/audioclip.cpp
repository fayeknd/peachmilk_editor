#include "audioclip.hpp"
#include "../editor/project.hpp"

void AudioClip::assignID() {
    if (m_created) return; 
    if (s_availableID <= s_largestIDused) s_availableID = s_largestIDused + 1;
    m_ID = s_availableID;
    s_largestIDused = s_availableID;
    s_availableID++;
}

AudioClip::AudioClip(const char* path, uint16_t mode, bool isStream) {
    if (!createSound(path, mode, isStream)) return;
}
AudioClip::~AudioClip() {
    for (int i = 0; i < s_allClips.size(); i++) {
        if (s_allClips[i] == this) { 
            s_allClips.erase(s_allClips.begin() + i);
            return;
        }
    }
}

bool AudioClip::createSound(const char* path, uint16_t mode, bool isStream, bool _assignID) {
    if (m_created) return false;
    if (!File::fileExists(path)) return false;
    std::string s = path;
    std::filesystem::path p(s);
    int tries = 0;
    std::string ext = File::getExtension(path);
    if (p.parent_path() != Project::get()->getSoundFolder()) {
        m_name = p.filename().string();
        s = std::string(Project::get()->getSoundFolder()) + m_name; 
        if (!File::fileExists(s))
            std::filesystem::copy_file(path, s);
        m_path = s;
    }
    for (int i = 0; i < s_allClips.size(); i++) {
        if (s_allClips[i]->m_path == path) {
            std::cout << "ERR : AudioClip already exists!" << std::endl;
            return false;
        }
    }
    AudioManager & am = AudioManager::get();
    if (isStream)
        am.getResult(FMOD_System_CreateStream(am.getSystem(), path, FMOD_NONBLOCKING, &am.m_exInfo, &m_sound));
    else
        am.getResult(FMOD_System_CreateSound(am.getSystem(), path, mode, &am.m_exInfo, &m_sound));
    if (_assignID) assignID(); 
    m_created = true;
    m_mode = mode; 
    m_isStream = isStream;
    s_allClips.push_back(this);
    return true;
}

AudioClip* AudioClip::getClipViaID(unsigned int id) {
    for (int i = 0; i < s_allClips.size(); i++) {
        if (s_allClips[i]->m_ID == id) return s_allClips[i];
    }
    return nullptr;
} 

void AudioClip::deseralizeSounds(std::string cache) {
    if (cache == "") cache = Project::get()->getSoundFolder();
    for (auto entry : std::filesystem::directory_iterator(cache)) {
        std::string file = entry.path().string();
        std::string ext(File::getExtension(file));
        if (ext == SND_DEFAULT_EXT) {
            std::ifstream os(file, std::ios::binary);
            {
                cereal::JSONInputArchive iarchive(os); // Create an output archive

                AudioClip* clip = new AudioClip;
                iarchive(*clip);
                clip->m_serializedPath = file;
                if (clip->m_ID > s_largestIDused) s_largestIDused = clip->m_ID;
                if (clip->m_path != "")
                    clip->createSound(clip->m_path.c_str(), clip->m_mode, clip->m_isStream, false);
            } 
        }  
    }
}
void AudioClip::serializeSounds(std::string cache) {
    if (cache == "") cache = Project::get()->getSoundFolder();
    for (int i = 0; i < s_allClips.size(); i++) {
        if (s_allClips[i]->m_serialize){
            std::string file = s_allClips[i]->m_serializedPath;
            if (file == "")
                file = std::string(cache) + std::string("\\sound") + std::to_string(i) + std::string(".") + SND_DEFAULT_EXT;
            std::ofstream os(file, std::ios::binary);
            {
                cereal::JSONOutputArchive oarchive(os); // Create an output archive
                oarchive(*s_allClips[i]);
            }
        }
    }
}