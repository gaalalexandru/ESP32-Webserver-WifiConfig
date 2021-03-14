set(srcs 
	"auxiliary.c"
	"main.c"
	"storage_handler.c"
    "webserver_handler.c"
	"wifi_handler.c"
	)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include")

# Create a SPIFFS image from the contents of the 'spiffs_content' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'. 
spiffs_create_partition_image(storage ../spiffs_content FLASH_IN_PROJECT)