# https://makefiletutorial.com/
# `pkg-config opencv4 --libs`
# `pkg-config opencv4 --cflags`
# `pkg-config opencv4 --cflags --libs`

TARGET_EXEC := demo1
BUILD_DIR := ./build
SRC_DIRS := ./

#INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS := .
ifeq ($(OS),Windows_NT)
	INC_FLAGS := $(addprefix -I,$(INC_DIRS))
	CFLAGS := $(INC_FLAGS) -g -MMD -MP `pkg-config opencv4 --cflags`
#TODO: Only include necessary libs
	LDFLAGS := -lws2_32
	LDFLAGS += `pkg-config opencv4 --libs-only-L`
	LDFLAGS += `pkg-config opencv4 --libs-only-l`
#LDFLAGS += -lopencv_gapi -lopencv_stitching -lopencv_alphamat -lopencv_aruco -lopencv_barcode -lopencv_bgsegm -lopencv_ccalib -lopencv_cvv -lopencv_dnn_objdetect -lopencv_dnn_superres -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_hfs -lopencv_img_hash -lopencv_intensity_transform -lopencv_line_descriptor -lopencv_mcc -lopencv_ovis -lopencv_quality -lopencv_rapid -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_sfm -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_superres -lopencv_optflow -lopencv_surface_matching -lopencv_tracking -lopencv_highgui -lopencv_datasets -lopencv_text -lopencv_plot -lopencv_videostab -lopencv_videoio -lopencv_viz -lopencv_wechat_qrcode -lopencv_xfeatures2d -lopencv_shape -lopencv_ml -lopencv_ximgproc -lopencv_video -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_dnn -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_imgproc -lopencv_core
else
	INC_FLAGS := $(addprefix -I,$(INC_DIRS))
	CFLAGS := $(INC_FLAGS) -g -MMD -MP `pkg-config opencv4 --cflags`
	LDFLAGS := `pkg-config opencv4 --libs`
endif

SRCS_CV := deps/opencv/cvmats.cpp \
           deps/opencv/draw.cpp \
		   deps/opencv/VideoReader.cpp \
		   deps/opencv/motionest_phasecorr.cpp \
		   deps/opencv/motionest_lucaskanade.cpp

SRCS := main.c flecs.c EgMotion.c EgTensors.c EgVideo.c EgTypes.c EgDraws.c tensor.c $(SRCS_CV)


OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c $< -o $@

all: $(BUILD_DIR)/$(TARGET_EXEC)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)