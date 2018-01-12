#pragma once

#include <mutex>
#include <unordered_map>

#include "dxvk_binding.h"
#include "dxvk_constant_state.h"
#include "dxvk_hash.h"
#include "dxvk_pipelayout.h"
#include "dxvk_resource.h"
#include "dxvk_shader.h"

namespace dxvk {
  
  /**
   * \brief Graphics pipeline state info
   * 
   * Stores all information that is required to create
   * a graphics pipeline, except the shader objects
   * themselves. Also used to identify pipelines using
   * the current pipeline state vector.
   */
  struct DxvkGraphicsPipelineStateInfo {
    DxvkGraphicsPipelineStateInfo();
    DxvkGraphicsPipelineStateInfo(
      const DxvkGraphicsPipelineStateInfo& other);
    
    DxvkGraphicsPipelineStateInfo& operator = (
      const DxvkGraphicsPipelineStateInfo& other);
    
    bool operator == (const DxvkGraphicsPipelineStateInfo& other) const;
    bool operator != (const DxvkGraphicsPipelineStateInfo& other) const;
    
    DxvkBindingState                    bsBindingState;
    
    VkPrimitiveTopology                 iaPrimitiveTopology;
    VkBool32                            iaPrimitiveRestart;
    
    uint32_t                            ilAttributeCount;
    uint32_t                            ilBindingCount;
    VkVertexInputAttributeDescription   ilAttributes[DxvkLimits::MaxNumVertexAttributes];
    VkVertexInputBindingDescription     ilBindings[DxvkLimits::MaxNumVertexBindings];
    
    VkBool32                            rsEnableDepthClamp;
    VkBool32                            rsEnableDiscard;
    VkPolygonMode                       rsPolygonMode;
    VkCullModeFlags                     rsCullMode;
    VkFrontFace                         rsFrontFace;
    VkBool32                            rsDepthBiasEnable;
    float                               rsDepthBiasConstant;
    float                               rsDepthBiasClamp;
    float                               rsDepthBiasSlope;
    uint32_t                            rsViewportCount;
    
    VkSampleCountFlagBits               msSampleCount;
    uint32_t                            msSampleMask;
    VkBool32                            msEnableAlphaToCoverage;
    VkBool32                            msEnableAlphaToOne;
    VkBool32                            msEnableSampleShading;
    float                               msMinSampleShading;
    
    VkBool32                            dsEnableDepthTest;
    VkBool32                            dsEnableDepthWrite;
    VkBool32                            dsEnableDepthBounds;
    VkBool32                            dsEnableStencilTest;
    VkCompareOp                         dsDepthCompareOp;
    VkStencilOpState                    dsStencilOpFront;
    VkStencilOpState                    dsStencilOpBack;
    float                               dsDepthBoundsMin;
    float                               dsDepthBoundsMax;
    
    VkBool32                            omEnableLogicOp;
    VkLogicOp                           omLogicOp;
    VkRenderPass                        omRenderPass;
    VkPipelineColorBlendAttachmentState omBlendAttachments[DxvkLimits::MaxNumRenderTargets];
  };
  
  
  /**
   * \brief Graphics pipeline
   * 
   * Stores the pipeline layout as well as methods to
   * recompile the graphics pipeline against a given
   * pipeline state vector.
   */
  class DxvkGraphicsPipeline : public DxvkResource {
    
  public:
    
    DxvkGraphicsPipeline(
      const Rc<vk::DeviceFn>& vkd,
      const Rc<DxvkShader>&   vs,
      const Rc<DxvkShader>&   tcs,
      const Rc<DxvkShader>&   tes,
      const Rc<DxvkShader>&   gs,
      const Rc<DxvkShader>&   fs);
    ~DxvkGraphicsPipeline();
    
    /**
     * \brief Pipeline layout
     * 
     * Stores the pipeline layout and the descriptor set
     * layout, as well as information on the resource
     * slots used by the pipeline.
     * \returns Pipeline layout
     */
    Rc<DxvkBindingLayout> layout() const {
      return m_layout;
    }
    
    /**
     * \brief Pipeline handle
     * \returns Pipeline handle
     */
    VkPipeline getPipelineHandle(
      const DxvkGraphicsPipelineStateInfo& state);
    
  private:
    
    struct PipelineStruct {
      DxvkGraphicsPipelineStateInfo stateVector;
      VkPipeline                    pipeline;
    };
    
    Rc<vk::DeviceFn>      m_vkd;
    Rc<DxvkBindingLayout> m_layout;
    
    Rc<DxvkShaderModule>  m_vs;
    Rc<DxvkShaderModule>  m_tcs;
    Rc<DxvkShaderModule>  m_tes;
    Rc<DxvkShaderModule>  m_gs;
    Rc<DxvkShaderModule>  m_fs;
    
    uint32_t m_vsIn  = 0;
    uint32_t m_fsOut = 0;
    
    std::mutex                  m_mutex;
    std::vector<PipelineStruct> m_pipelines;
    
    VkPipeline m_basePipeline = VK_NULL_HANDLE;
    
    VkPipeline compilePipeline(
      const DxvkGraphicsPipelineStateInfo& state,
            VkPipeline                     baseHandle) const;
    
    void destroyPipelines();
    
    bool validatePipelineState(
      const DxvkGraphicsPipelineStateInfo& state) const;
    
  };
  
}