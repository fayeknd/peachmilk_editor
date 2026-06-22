#include "channelgroup.hpp"
#include "../editor/project.hpp"

void ChannelGroup::setFalloff(float minf, float maxf) {
    m_minFalloff = minf;
    m_maxFalloff = maxf;
    for (int i = 0; i < m_channels.size(); i++) {
        AudioSource* source = m_channels[i];
        source->setFalloff(source->getMinFalloff(), m_minFalloff, source->getMaxFalloff(), m_maxFalloff);
    }
    updateChannelMultipliers();
}

void ChannelGroup::setDoppler(float d) {
    m_doppler = d;
    for (int i = 0; i < m_channels.size(); i++) {
        AudioSource* source = m_channels[i];
        source->setDoppler(source->getDoppler(), m_doppler);
    }
    updateChannelMultipliers();
}

void ChannelGroup::setPitch(float p) {
    m_pitch = p;
    for (int i = 0; i < m_channels.size(); i++) {
        AudioSource* source = m_channels[i];
        source->setPitch(source->getPitch(), m_pitch);
    }
    updateChannelMultipliers();
}

void ChannelGroup::setVolume(float v) {
    m_volume = v;
    for (int i = 0; i < m_channels.size(); i++) {
        AudioSource* source = m_channels[i];
        source->setVolume(source->getVolume(), m_volume);
    }
    updateChannelMultipliers();
}

void ChannelGroup::set3DLevel(float l) {
    m_3dLevel = l;
    for (int i = 0; i < m_channels.size(); i++) {
        AudioSource* source = m_channels[i];
        source->set3DLevel(source->getDoppler(), m_3dLevel);
    }
    updateChannelMultipliers();
}

bool ChannelGroup::create(std::string name, bool skipNameCheck) {
    if (m_created) return false;

    if (!skipNameCheck) {
        for (int i = 0; i < s_channelGroups.size(); i++) {
            if (name == s_channelGroups[i]->m_name) {
                std::cout << "ERR : Channel not created. Channel " << s_channelGroups[i]->m_ID << " already has that name! ("<< name << ")" << std::endl;
                return false;
            }
        }
    }

    setFalloff(m_minFalloff, m_maxFalloff);
    setDoppler(m_doppler);
    setPitch(m_pitch);
    setVolume(m_volume);
    set3DLevel(m_3dLevel);

    m_name = name;
    m_ID = s_availableID;
    s_availableID++;

    m_created = true;
    return true;
} 

ChannelGroup::ChannelGroup() {
    s_channelGroups.push_back(this);
}

ChannelGroup::ChannelGroup(std::string name) {
    s_channelGroups.push_back(this);
    create(name);
}

ChannelGroup::~ChannelGroup() {
    for (int i = 0; i < m_channels.size(); i++) {
        m_channels[i]->m_channelGroupName = "";
    }
    for (int i = 0; i < s_channelGroups.size(); i++) {
        if (s_channelGroups[i] == this) {
            s_channelGroups.erase(s_channelGroups.begin() + i);
            break;
        }
    }
}

ChannelGroup * ChannelGroup::getChannelGroup(unsigned int id) {
    for (int i = 0; i < s_channelGroups.size(); i++) {
        if (s_channelGroups[i]->m_ID == id) return s_channelGroups[i];
    }
    return nullptr;
}

ChannelGroup * ChannelGroup::getChannelGroup(std::string name) {
    for (int i = 0; i < s_channelGroups.size(); i++) {
        if (s_channelGroups[i]->m_name == name) return s_channelGroups[i];
    }
    return nullptr;
}

void ChannelGroup::deseralizeChannelGroups(std::string cache) {
    if (cache == "") cache = Project::get()->getChannelGroupFolder();
    for (auto entry : std::filesystem::directory_iterator(cache)) {
        std::string file = entry.path().string();
        std::string ext(File::getExtension(file));
        if (ext == CHN_DEFAULT_EXT) {
            std::ifstream os(file, std::ios::binary);
            {
                cereal::JSONInputArchive iarchive(os); // Create an output archive

                ChannelGroup * cg = new ChannelGroup;
                iarchive(*cg);
                cg->m_serializedPath = file;
                cg->create(cg->m_name, true); 
            } 
        }  
    }
}
void ChannelGroup::serializeChannelGroups(std::string cache) {
    if (cache == "") cache = Project::get()->getChannelGroupFolder();
    for (int i = 0; i < s_channelGroups.size(); i++) {
        if (s_channelGroups[i]->m_serialize) {
            std::string file = s_channelGroups[i]->m_serializedPath;
            if (file == "")
                file = std::string(cache) + std::string("\\channelgroup") + std::to_string(i) + std::string(".") + CHN_DEFAULT_EXT;
            std::ofstream os(file, std::ios::binary);
            {
                cereal::JSONOutputArchive oarchive(os); // Create an output archive
                oarchive(*s_channelGroups[i]);
            }
        }
    }
}

void ChannelGroup::updateChannelMultipliers() {
    for(int i = 0; i < m_channels.size(); i++) {
        AudioSource* audio = m_channels[i];
        audio->m_volume_mult = m_volume;
        audio->m_3dLevel_mult = m_3dLevel;
        audio->m_pitch_mult = m_pitch;
        audio->m_doppler_mult = m_doppler;
        audio->m_minFalloff_mult = m_minFalloff;
        audio->m_maxFalloff_mult = m_maxFalloff;
        audio->m_isDirty = true;
    }
}