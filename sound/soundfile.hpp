/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
int SoundFile::numChannels() const
{
    return m_channels.size();
}

//------------------------------------------------------------------------------
const double & SoundFile::startTime() const
{
    return m_start_time;
}

//------------------------------------------------------------------------------
void SoundFile::setStartTime(const double & p_start_time)
{
    m_start_time = p_start_time;
}

//------------------------------------------------------------------------------
int SoundFile::currentStreamChunk() const
{
    return (m_stream->pos() - offset()) / framesPerChunk();
}

//------------------------------------------------------------------------------
int SoundFile::currentRawChunk() const
{
    return m_chunk_num;
}

//------------------------------------------------------------------------------
int SoundFile::currentChunk() const
{
    return m_chunk_num;
}

//------------------------------------------------------------------------------
void SoundFile::setCurrentChunk(int p_x)
{
    m_chunk_num = p_x;
}

//------------------------------------------------------------------------------
void SoundFile::incrementChunkNum()
{
    m_chunk_num++;
}

//------------------------------------------------------------------------------
int SoundFile::offset() const
{
    return m_offset;
}

//------------------------------------------------------------------------------
double SoundFile::timePerChunk() const
{
    return double(framesPerChunk()) / double(rate());
}

//------------------------------------------------------------------------------
int SoundFile::chunkAtTime(const double & p_time) const
{
    return toInt(chunkFractionAtTime(p_time));
} //this is not bounded

//------------------------------------------------------------------------------
double SoundFile::chunkFractionAtTime(const double & p_time) const
{
    return p_time / timePerChunk();
} //this is not bounded

//------------------------------------------------------------------------------
double SoundFile::timeAtChunk(int p_chunk) const
{
    return double(p_chunk) * timePerChunk();
}

//------------------------------------------------------------------------------
double SoundFile::timeAtCurrentChunk() const
{
    return timeAtChunk(currentChunk());
}

//------------------------------------------------------------------------------
int SoundFile::chunkAtCurrentTime() const
{
    return chunkAtTime(g_data->getView().currentTime());
}

//------------------------------------------------------------------------------
void SoundFile::jumpToTime(const double & p_time)
{
    jumpToChunk(chunkAtTime(p_time));
}

//------------------------------------------------------------------------------
int SoundFile::rate() const
{
    myassert(m_stream != nullptr);
    return m_stream->get_frequency();
}

//------------------------------------------------------------------------------
int SoundFile::bits() const
{
    myassert(m_stream != nullptr);
    return m_stream->get_bits();
}

//------------------------------------------------------------------------------
int SoundFile::framesPerChunk() const
{
    return m_frames_per_chunk;
}

//------------------------------------------------------------------------------
void SoundFile::setFramesPerChunk(int p_step_size)
{
    m_frames_per_chunk = p_step_size;
}

//------------------------------------------------------------------------------
bool SoundFile::saved() const
{
    return m_saved;
}

//------------------------------------------------------------------------------
void SoundFile::setSaved(bool p_new_state)
{
    m_saved = p_new_state;
}

//------------------------------------------------------------------------------
bool SoundFile::equalLoudness() const
{
    return m_my_transforms.getEqualLoudness();
}

//------------------------------------------------------------------------------
bool SoundFile::doingDetailedPitch() const
{
    return m_doing_detailed_pitch;
}

//------------------------------------------------------------------------------
bool SoundFile::isFirstTimeThrough() const
{
    return m_first_time_through;
}

//------------------------------------------------------------------------------
const std::string & SoundFile::getFileName() const
{
    return m_filename;
}

//------------------------------------------------------------------------------
void SoundFile::setFirstTimeThrough(bool p_firstTimeThrough)
{
    m_first_time_through = p_firstTimeThrough;
}

//------------------------------------------------------------------------------
void SoundFile::calculateAnalysisData(int p_chunk, Channel *p_channel)
{
    m_my_transforms.calculateAnalysisData(p_chunk,p_channel);
}

//------------------------------------------------------------------------------
int SoundFile::getChannelIndex(const Channel & p_channel)const
{
    for(int i = 0; i < m_channels.size(); ++i)
    {
        if(&p_channel == m_channels.at(i))
        {
            return i;
        }
    }
    // If we're here, we didn't find the channel
    // This should never happen!
    myassert(false);
    return -1;
}

//------------------------------------------------------------------------------
Channel & SoundFile::getChannel(int p_index)
{
    return *(m_channels.at(p_index));
}

//------------------------------------------------------------------------------
SoundFileStream & SoundFile::getStream()
{
    assert(m_stream);
    return *m_stream;
}

//EOF
